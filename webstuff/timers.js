// This function can be used to determine when the page has finished
// updating after a resize event.
var waitForFinalEvent = (function () {
  var timers = {};
  return function (callback, ms, uniqueId) {
    if (!uniqueId) {
      uniqueId = "Don't call this twice without a uniqueId";
    }
    if (timers[uniqueId]) {
      clearTimeout (timers[uniqueId]);
    }
    timers[uniqueId] = setTimeout(callback, ms);
  };
})();



var updateStateTimer;

// Regularly loads state information from the supervisor.
// Once the update has been completed succesfully, sendCommand() will
// call this method again.
function updateState(ms) {
	clearTimeout(updateStateTimer);
	
	updateStateTimer = setTimeout(function() {
		sendCommand({
			cmd: "update"
		});
	}, ms);
}

function clearStateUpdateTimer() {
	clearTimeout(updateStateTimer);
}




var enableCameraUpdates = false;
var cameraID = 1;

function updateCameras() {
	var date = new Date();
	var image = new Image();
	
	// Appending # to the URL prevents browsers from fetching cached content,
	// and ensures an updated image
	var imageSrc = "cameras/camera" + cameraID + ".jpg#" + date.getTime();
	var imageTarget = "#camera" + cameraID;
	
	// There are four images to update
	cameraID++;
	if (cameraID > 4) {
		cameraID = 1;
	}	
	
	// After image has completed loading, schedule the next update.
	image.onload = function() {
		$(imageTarget).attr("src", this.src);
		
		if (updateCameras) {
			// By not calling the function directly, we avoid infinite
			// recursion, and possibly a stack overflow.
			setTimeout(updateCameras, 1);
		}
	};
	
	image.src = imageSrc;
}


// Add some logic so that it is not possible to have multiple updates
// occuring simultaneously.
function startUpdatingCameras() {
	if (!enableCameraUpdates) {
		enableCameraUpdates = true;
		updateCameras();
	}
}

function stopUpdatingCameras() {
	enableCameraUpdates = false;
}
