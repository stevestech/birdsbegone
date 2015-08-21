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
