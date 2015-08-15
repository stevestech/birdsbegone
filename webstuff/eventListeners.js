$(window).on("load", function() {
	updateControlsWidth();
	
	// update isn't a registered command on the supervisor, but this
	// will still result in the supervisor sending the state to the
	// web client, it does so for all commands.
	sendCommand({ cmd: "update" });
});



$(window).on("resize", function() {
	waitForFinalEvent(updateControlsWidth, 100, "unique1");
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
	
	
	
	// Switch between interface tabs
	$("#tabs").on("tabsactivate", function(event, ui) {
		switch(ui.newTab.text()) {
			// Start updating cameras
			case "Cameras":
				waitThenUpdateCamera();
			break;
			
			// Start updating state
			case "Testing":
				waitThenUpdateState();
			break;
			
			default:
			break;
		}
		
		switch(ui.oldTab.text()) {
			// Stop updating cameras
			case "Cameras":
				clearCameraUpdateTimers();
			break;
			
			// Stop updating state
			case "Testing":
				clearStateUpdateTimers();
			break;
			
			default:
			break;
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
		if ($("#throttle .slider").hasClass("do-not-broadcast")) {
			$("#throttle .slider").removeClass("do-not-broadcast");
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
		if ($("#fl-angle .slider").hasClass("do-not-broadcast")) {
			$("#fl-angle .slider").removeClass("do-not-broadcast");
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
