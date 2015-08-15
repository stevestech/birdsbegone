// Initialisation:
// Hook jquery-ui onto the appropriate elements
$(function() {
	$("#tabs").tabs();
	
	$("button").button();
	
	$("#states").buttonset();
	
	$(".buttonset").buttonset();
	
	$("#throttle .slider").slider({
		value: 0,
		min: 0,
		max: 255,
		animate: "slow"
	});
	
	$("#fl-angle .slider").slider({
		value: 0,
		min: 0,
		max: 1023,
		animate: "slow"
	});
	
	$("#fl-angle-pot .bar").progressbar({
		value: 0,
		max: 1023
	});
	
	$("#fl-actuator .bar").progressbar({
		value: 0,
		max: 510
	});
});


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
	


// After the state has been loaded from the supervisor, update the
// controls with new information
function updateControls(data) {

	switch(data["fl-state"]) {
		case "neutral":
			$("#manual-states-neutral").prop("checked", true);
			$("#states").buttonset("refresh");
			break;
			
		case "braking":
			$("#manual-states-braking").prop("checked", true);		
			$("#states").buttonset("refresh");
			break;
			
		case "forward":
			$("#manual-states-forward").prop("checked", true);		
			$("#states").buttonset("refresh");
			break;
		
		case "reverse":
			$("#manual-states-reverse").prop("checked", true);		
			$("#states").buttonset("refresh");
			break;
	}

	// Sliders should not broadcast this value change back to the
	// supervisor, or a looping mess of sockets would result
	$("#throttle .slider").addClass("updated");
	$("#fl-angle .slider").addClass("updated");

	$("#throttle .slider").slider("value", data["fl-throttle"]);
	$("#fl-angle .slider").slider("value", data["fl-angle"]);
	
	$("#fl-angle-pot .value").text(data["fl-angle-pot"]);
	$("#fl-angle-pot .bar").progressbar("value", data["fl-angle-pot"]);
	
	$("#fl-actuator .value").text(data["fl-actuator"]);
	$("#fl-actuator .bar").progressbar("value", data["fl-actuator"] + 255);
}


function updateCameras() {
	var d = new Date();
	$("#camera1").attr("src", "cameras/camera1.jpg#" + d.getTime());
}


// Sends a command to the supervisor, and parses the response. Also
// displays any errors that are encountered.
function sendCommand(commands) {
	// This method is already being called, so there is no need for any timers
	// to call it again. A new timer will be generated after the this method
	// finishes communicating with the supervisor.
	clearStateUpdateTimers();
	
	$.ajax({
		url: "/cgi-bin/command.cgi",
		data: commands,
		dataType: "json",
		success: function(data, status, jqHXR) {
				
			if ("error" in data) {
				setStatus(true, data["error"]);
				waitThenUpdateState(10000);
			}
			
			else {
				setStatus(false);
				updateControls(data);
				waitThenUpdateState(1000);
			}
			
		},
		
		error: function(jqHXR, status, error) {	
			setStatus(true, "Something went wrong with the command.cgi script.");
			waitThenUpdateState(10000);
		}
	});
}	


// Set width of sliders based on width of existing elements
function updateControlsWidth() {
	var width = $("#throttle").width();

	var sliderWidth = width - 380;
	sliderWidth = sliderWidth + "px";
	
	var progressBarWidth = width - 185;
	progressBarWidth = progressBarWidth + "px";
	
	$(".slider").css("width", sliderWidth);
	$(".bar").css("width", progressBarWidth);
}
