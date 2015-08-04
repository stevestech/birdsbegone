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



// Global to keep track of whether the status is hidden
var statusDisplayed = true;

function setStatus(show, message) {
	if (show && statusDisplayed) {
		$("#status").text(message);
	}
	
	else if (show && !statusDisplayed) {
		$("#status").text(message);
		$("#status").toggle("blind");
		statusDisplayed = true;
	}
	
	else if (!show && statusDisplayed) {
		$("#status").toggle("blind");
		statusDisplayed = false;
	}
}
	
	

// Sends a command to the supervisor, and parses the response. Also
// displays any errors that are encountered.
function sendCommand(commands) {
	$.ajax({
		url: "/cgi-bin/command.cgi",
		data: commands,
		dataType: "json",
		success: function(data, status, jqHXR) {
			
			if (data["exception"] == false) {
				setStatus(false);
			}
			
			else {
				
				if (data["exception"] == "[Errno 111] Connection refused") {
					setStatus(true, "The robot supervisor program is not running.");
				}
				
				else {
					setStatus(true, data["exception"]);
				}
				
			}
			
			// TODO: Update controls based on the returned state of the supervisor
			//$( "#manual-states-neutral" ).prop("checked", true);
			//$( ".manual .states" ).buttonset("refresh");
			
		},
		
		error: function(jqHXR, status, error) {	
			setStatus(true, "Something went wrong with the command.cgi script.");
		}
	});
}	

// Hook jquery-ui onto the appropriate elements
$(function() {
	$("#manual").accordion({
		heightStyle: "content"
	});
	
	$("button").button();
	
	$("#states").buttonset();
	
	$(".buttonset").buttonset();
	
	$("#throttle .slider").slider({
		value: 0,
		min: 0,
		max: 100,
		animate: "slow"
	});
	
	$("#fl-angle .slider").slider({
		value: 0,
		min: -360,
		max: 360,
		step: 15,
		animate: "slow"
	});
});

// Register event listeners
$(function() {
	
	// Emergency stop button
	$("#stop").mousedown(function(event) {
		// Left mouse button
		if(event.which === 1) {
			sendCommand({
				commandName: "setState",
				state: "neutral"
			});

			// This also triggers the slidechange event, sending the
			// throttle value to the supervisor.
			$("#throttle .slider").slider("value", 0);
		}
	});


	
	// Change state using the state radio buttons
	$("#states input").change(function() {
		sendCommand({
			commandName: "setState",
			state: this.value
		});
	});


	
	// Change throttle using the slider control
	$("#throttle .slider").on("slidechange", function(event, ui) {
		$("#throttle .value").text(ui.value);
		
		sendCommand({
			commandName: "setThrottle",
			wheel: "all",
			throttle: ui.value
		});
	});
	
	// Set throttle to zero using button
	$("#throttle .0").mousedown(function(event) {
		// Left mouse button
		if(event.which === 1) {
			$("#throttle .slider").slider("value", 0);
		}
	});
	
	// Decrease throttle using button
	$("#throttle .decrease").mousedown(function (event) {
		// Left mouse button
		if(event.which === 1) {
			var newValue = $("#throttle .slider").slider("value");
			newValue -= 10;
			
			$("#throttle .slider").slider("value", newValue);
		}
	});
	
	// Increase throttle using button
	$("#throttle .increase").mousedown(function (event) {
		// Left mouse button
		if(event.which === 1) {
			var newValue = $("#throttle .slider").slider("value");
			newValue += 10;
			
			$("#throttle .slider").slider("value", newValue);
		}
	});
	
	
	
	// Change FL-angle using the slider control
	$("#fl-angle .slider").on("slidechange", function(event, ui) {
		$("#fl-angle .value").text(ui.value + "°");
		
		sendCommand({
			commandName: "setAngle",
			wheel: "fl",
			angle: ui.value
		});
	});
	
	// Set throttle to zero using button
	$("#fl-angle .0").mousedown(function(event) {
		// Left mouse button
		if(event.which === 1) {
			$("#fl-angle .slider").slider("value", 0);
		}
	});
	
	// Decrease throttle using button
	$("#fl-angle .decrease").mousedown(function (event) {
		// Left mouse button
		if(event.which === 1) {
			var newValue = $("#fl-angle .slider").slider("value");
			newValue -= 15;
			
			$("#fl-angle .slider").slider("value", newValue);
		}
	});
	
	// Increase throttle using button
	$("#fl-angle .increase").mousedown(function (event) {
		// Left mouse button
		if(event.which === 1) {
			var newValue = $("#fl-angle .slider").slider("value");
			newValue += 15;
			
			$("#fl-angle .slider").slider("value", newValue);
		}
	});	
});


// Set width of sliders based on width of existing elements
function sizeSliders() {
	var width = $("#throttle").width();
	width -= 380;
	width = width + "px";
	
	$("#throttle .slider").css("width", width);
	$("#fl-angle .slider").css("width", width);
}

$(window).on("load", sizeSliders);

$(window).on("resize", function() {
	waitForFinalEvent(sizeSliders, 100, "unique1");
});
