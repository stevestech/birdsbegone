$(window).on("load", function() {
	updateControlsWidth();
	updateState(1);
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
				cmd: "setHmMode",
				wheel: "all",
				mode: "neutral"
			});

			// This also triggers the slidechange event, sending the
			// throttle value to the supervisor.
			$("#throttle .slider").slider("value", 0);
		}
	});
	
	
	
	// Switch between interface tabs
	$("#tabs").on("tabsactivate", function(event, ui) {
		// Perform initialisation tasks for the newly selected tab
		switch(ui.newTab.text()) {
			case "Cameras":
				startUpdatingCameras();
			break;
			
			default:
			break;
		}
		
		// Perform cleanup for the previous tab
		switch(ui.oldTab.text()) {
			case "Cameras":
				stopUpdatingCameras();
			break;
			
			default:
			break;
		}
	});


	
	// Change state using the state radio buttons
	$("#states input").change(function() {
		sendCommand({
			cmd: "setHmMode",
			wheel: "all",
			mode: this.value
		});
	});


	
	// Change throttle using the slider control
	$("#throttle .slider").on("slidechange", function(event, ui) {
		$("#throttle .value").text(ui.value);
		
		// Should we broadcast this value change to the supervisor?
		if ($("#throttle .slider").hasClass("no-broadcast")) {
			$("#throttle .slider").removeClass("no-broadcast");
		}
		
		else {
			sendCommand({
				cmd: "setHmThrottle",
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
		if ($("#fl-angle .slider").hasClass("no-broadcast")) {
			$("#fl-angle .slider").removeClass("no-broadcast");
		}
		
		else {
			sendCommand({
				cmd: "setActAngle",
				wheel: "all",
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
