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
		$("#error_message").text(message);
	}
	
	else if (show && !statusDisplayed) {
		$("#error_message").text(message);
		$("#error_message").toggle("blind");
		statusDisplayed = true;
	}
	
	else if (!show && statusDisplayed) {
		$("#error_message").toggle("blind");
		statusDisplayed = false;
	}
}
	


// After the state has been loaded from the supervisor, update the
// controls with new information
function updateControls(data) {
	switch(data["WHEELS"]["BACK_LEFT"]["HM_MODE"]) {
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

	$("#throttle .slider").slider("value", data["WHEELS"]["BACK_LEFT"]["HM_THROTTLE"]);
	$("#fl-angle .slider").slider("value", data["WHEELS"]["BACK_LEFT"]["A_ANGLE_D"]);
	
	$("#fl-angle-pot .value").text(data["WHEELS"]["BACK_LEFT"]["A_ANGLE_M"]);
	$("#fl-angle-pot .bar").progressbar("value", data["WHEELS"]["BACK_LEFT"]["A_ANGLE_M"]);
	
	$("#fl-actuator .value").text(data["WHEELS"]["BACK_LEFT"]["A_THROTTLE"]);
	$("#fl-actuator .bar").progressbar("value", data["WHEELS"]["BACK_LEFT"]["A_THROTTLE"] + 127);
	
	if (data["ERRORS"]["MSG"] != "") {
		setStatus(true, data["ERRORS"]["MSG"]);
	}
	
	else {
		setStatus(false, "");
	}
	
	$(".fl .slave_echo_failed").text(data["ERRORS"]["FRONT_LEFT"]["SLAVE_ECHO_FAILED"]);
	$(".fl .missing_nul_terminator").text(data["ERRORS"]["FRONT_LEFT"]["MISSING_NUL_TERMINATOR"]);
	$(".fl .message_has_forbidden_chars").text(data["ERRORS"]["FRONT_LEFT"]["MESSAGE_HAS_FORBIDDEN_CHARS"]);
	$(".fl .command_not_recognised").text(data["ERRORS"]["FRONT_LEFT"]["COMMAND_NOT_RECOGNISED"]);
	$(".fl .not_ready").text(data["ERRORS"]["FRONT_LEFT"]["NOT_READY"]);
	$(".fl .master_echo_failed").text(data["ERRORS"]["FRONT_LEFT"]["MASTER_ECHO_FAILED"]);
	$(".fl .total").text(data["ERRORS"]["FRONT_LEFT"]["NUM_TRANSFERS"]);
	
	$(".fr .slave_echo_failed").text(data["ERRORS"]["FRONT_RIGHT"]["SLAVE_ECHO_FAILED"]);
	$(".fr .missing_nul_terminator").text(data["ERRORS"]["FRONT_RIGHT"]["MISSING_NUL_TERMINATOR"]);
	$(".fr .message_has_forbidden_chars").text(data["ERRORS"]["FRONT_RIGHT"]["MESSAGE_HAS_FORBIDDEN_CHARS"]);
	$(".fr .command_not_recognised").text(data["ERRORS"]["FRONT_RIGHT"]["COMMAND_NOT_RECOGNISED"]);
	$(".fr .not_ready").text(data["ERRORS"]["FRONT_RIGHT"]["NOT_READY"]);
	$(".fr .master_echo_failed").text(data["ERRORS"]["FRONT_RIGHT"]["MASTER_ECHO_FAILED"]);
	$(".fr .total").text(data["ERRORS"]["FRONT_RIGHT"]["NUM_TRANSFERS"]);
	
	$(".bl .slave_echo_failed").text(data["ERRORS"]["BACK_LEFT"]["SLAVE_ECHO_FAILED"]);
	$(".bl .missing_nul_terminator").text(data["ERRORS"]["BACK_LEFT"]["MISSING_NUL_TERMINATOR"]);
	$(".bl .message_has_forbidden_chars").text(data["ERRORS"]["BACK_LEFT"]["MESSAGE_HAS_FORBIDDEN_CHARS"]);
	$(".bl .command_not_recognised").text(data["ERRORS"]["BACK_LEFT"]["COMMAND_NOT_RECOGNISED"]);
	$(".bl .not_ready").text(data["ERRORS"]["BACK_LEFT"]["NOT_READY"]);
	$(".bl .master_echo_failed").text(data["ERRORS"]["BACK_LEFT"]["MASTER_ECHO_FAILED"]);
	$(".bl .total").text(data["ERRORS"]["BACK_LEFT"]["NUM_TRANSFERS"]);
	
	$(".br .slave_echo_failed").text(data["ERRORS"]["BACK_RIGHT"]["SLAVE_ECHO_FAILED"]);
	$(".br .missing_nul_terminator").text(data["ERRORS"]["BACK_RIGHT"]["MISSING_NUL_TERMINATOR"]);
	$(".br .message_has_forbidden_chars").text(data["ERRORS"]["BACK_RIGHT"]["MESSAGE_HAS_FORBIDDEN_CHARS"]);
	$(".br .command_not_recognised").text(data["ERRORS"]["BACK_RIGHT"]["COMMAND_NOT_RECOGNISED"]);
	$(".br .not_ready").text(data["ERRORS"]["BACK_RIGHT"]["NOT_READY"]);
	$(".br .master_echo_failed").text(data["ERRORS"]["BACK_RIGHT"]["MASTER_ECHO_FAILED"]);
	$(".br .total").text(data["ERRORS"]["BACK_RIGHT"]["NUM_TRANSFERS"]);
	
	$(".control .slave_echo_failed").text(data["ERRORS"]["POWER_CONTROL"]["SLAVE_ECHO_FAILED"]);
	$(".control .missing_nul_terminator").text(data["ERRORS"]["POWER_CONTROL"]["MISSING_NUL_TERMINATOR"]);
	$(".control .message_has_forbidden_chars").text(data["ERRORS"]["POWER_CONTROL"]["MESSAGE_HAS_FORBIDDEN_CHARS"]);
	$(".control .command_not_recognised").text(data["ERRORS"]["POWER_CONTROL"]["COMMAND_NOT_RECOGNISED"]);
	$(".control .not_ready").text(data["ERRORS"]["POWER_CONTROL"]["NOT_READY"]);
	$(".control .master_echo_failed").text(data["ERRORS"]["POWER_CONTROL"]["MASTER_ECHO_FAILED"]);
	$(".control .total").text(data["ERRORS"]["POWER_CONTROL"]["NUM_TRANSFERS"]);
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
