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
	
function updateControls(data) {

	switch(data["fl-state"]) {
		case "neutral":
			$("#manual-states-neutral").prop("checked", true);
			$("#manual-states-braking").prop("checked", false);
			$("#manual-states-forward").prop("checked", false);
			$("#manual-states-reverse").prop("checked", false);
			$("#states").buttonset("refresh");
			break;
			
		case "braking":
			$("#manual-states-braking").prop("checked", true);		
			$("#manual-states-neutral").prop("checked", false);
			$("#manual-states-forward").prop("checked", false);
			$("#manual-states-reverse").prop("checked", false);
			$("#states").buttonset("refresh");
			break;
			
		case "forward":
			$("#manual-states-forward").prop("checked", true);		
			$("#manual-states-braking").prop("checked", false);		
			$("#manual-states-neutral").prop("checked", false);
			$("#manual-states-reverse").prop("checked", false);
			$("#states").buttonset("refresh");
			break;
		
		case "reverse":
			$("#manual-states-reverse").prop("checked", true);		
			$("#manual-states-forward").prop("checked", false);		
			$("#manual-states-braking").prop("checked", false);		
			$("#manual-states-neutral").prop("checked", false);
			$("#states").buttonset("refresh");
			break;
	}

	// Sliders should not broadcast this value change back to the
	// supervisor, or a looping mess of sockets would result
	$("#throttle .slider").addClass("updated");
	$("#fl-angle .slider").addClass("updated");

	$("#throttle .slider").slider("value", data["fl-throttle"]);
	$("#fl-angle .slider").slider("value", data["fl-angle"]);
}

// Sends a command to the supervisor, and parses the response. Also
// displays any errors that are encountered.
function sendCommand(commands) {
	$.ajax({
		url: "/cgi-bin/command.cgi",
		data: commands,
		dataType: "json",
		success: function(data, status, jqHXR) {
				
			if ("error" in data) {
				setStatus(true, data["error"]);
			}
			
			else {
				setStatus(false);
				updateControls(data);		
			}
			
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
				cmd: "setState",
				wheel: "all",
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
			cmd: "setState",
			wheel: "all",
			state: this.value
		});
	});


	
	// Change throttle using the slider control
	$("#throttle .slider").on("slidechange", function(event, ui) {
		$("#throttle .value").text(ui.value);
		
		// Should we broadcast this value change to the supervisor?
		if ($("#throttle .slider").hasClass("updated")) {
			$("#throttle .slider").removeClass("updated");
		}
		
		else {		
			sendCommand({
				cmd: "setThrottle",
				wheel: "all",
				throttle: ui.value
			});
		}
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
		
		// Should we broadcast this value change to the supervisor?
		if ($("#fl-angle .slider").hasClass("updated")) {
			$("#fl-angle .slider").removeClass("updated");
		}
		
		else {
			sendCommand({
				cmd: "setAngle",
				wheel: "fl",
				angle: ui.value
			});
		}
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

$(window).on("load", function() {
	sizeSliders();
	
	// update isn't a registered command on the supervisor, but this
	// will still result in the supervisor sending the state to the
	// web client, it does so for all commands.
	sendCommand({
		cmd: "update"
	});
});

$(window).on("resize", function() {
	waitForFinalEvent(sizeSliders, 100, "unique1");
});
