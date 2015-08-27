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
		max: 255
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
	switch(data["WHEELS"]["FRONT_LEFT"]["HM_MODE"]) {
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
	$("#throttle .slider").addClass("no-broadcast");
	$("#fl-angle .slider").addClass("no-broadcast");

	$("#throttle .slider").slider("value", data["WHEELS"]["FRONT_LEFT"]["HM_THROTTLE"]);
	$("#fl-angle .slider").slider("value", data["WHEELS"]["FRONT_LEFT"]["A_ANGLE_D"]);
	
	$("#fl-angle-pot .value").text(data["WHEELS"]["FRONT_LEFT"]["A_ANGLE_M"]);
	$("#fl-angle-pot .bar").progressbar("value", data["WHEELS"]["FRONT_LEFT"]["A_ANGLE_M"]);
	
	$("#fl-actuator .value").text(data["WHEELS"]["FRONT_LEFT"]["A_THROTTLE"]);
	$("#fl-actuator .bar").progressbar("value", data["A_THROTTLE"] + 127);
}


function updateCameras() {
	var d = new Date();
	$("#camera1").attr("src", "cameras/camera1.jpg#" + d.getTime());
}


// Sends a command to the supervisor, and parses the response. Also
// displays any errors that are encountered.
function sendCommand(commands) {
	// Prevents more than one state update being scheduled.
	clearStateUpdateTimer();
	
	$.ajax({
		url: "/cgi-bin/command.cgi",
		data: commands,
		dataType: "json",
		success: function(data, status, jqHXR) {
				
			if ("error" in data) {
				setStatus(true, data["error"]);
				updateState(10000);
			}
			
			else {
				setStatus(false);
				updateControls(data);
				updateState(1000);
			}
			
		},
		
		error: function(jqHXR, status, error) {	
			setStatus(true, "Something went wrong with the command.cgi script.");
			updateState(10000);
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


function startUpdatingCameras() {
	$("#camera1").attr("src", "http://192.168.1.100:8080/?action=stream");
}

function stopUpdatingCameras() {
	$("#camera1").attr("src", "");
}
