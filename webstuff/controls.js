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

// Hook jquery-ui onto the appropriate elements
$(function() {
	$( ".manual" ).accordion({
		heightStyle: "content"
	});
	
	$( "button" ).button();
	
	$( ".manual .states" ).buttonset();
	
	$( ".manual .throttle .buttonset" ).buttonset();
	$( ".manual .fl-angle .buttonset" ).buttonset();
	
	$( ".manual .throttle .slider" ).slider({
		value: 0,
		min: 0,
		max: 100,
		animate: "slow"
	});
	
	$( ".manual .fl-angle .slider" ).slider({
		value: 0,
		min: -360,
		max: 360,
		animate: "slow"
	});
});

// Register event listeners
$(function() {
	
	// Emergency stop button
	$( ".stop" ).mousedown(function(event) {
		// Left mouse button
		if(event.which === 1) {
			$.ajax("/cgi-bin/command.cgi?commandName=setState&state=neutral");

			// This also triggers the slidechange event, sending the
			// throttle value to the supervisor.
			$( ".manual .throttle .slider" ).slider("value", 0);
			
			$( "#manual-states-neutral" ).prop("checked", true);
			$( ".manual .states" ).buttonset("refresh");
		}
	});


	
	// Change state using the state radio buttons
	$( ".manual .states input[type=radio]" ).change(function() {
		alert("State = " + this.value);
		$.ajax("/cgi-bin/command.cgi?commandName=setState&state=" + this.value);
	});


	
	// Change throttle using the slider control
	$( ".manual .throttle .slider" ).on("slidechange", function( event, ui ) {
		$( ".manual .throttle .value" ).text(ui.value);
		$.ajax("/cgi-bin/command.cgi?commandName=setThrottle&throttle=" + ui.value);
	});
	
	// Set throttle to zero using button
	$( ".manual .throttle .buttonset .0" ).mousedown(function(event) {
		// Left mouse button
		if(event.which === 1) {
			$( ".manual .throttle .slider" ).slider("value", 0);
		}
	});
	
	// Decrease throttle using button
	$( ".manual .throttle .buttonset .decrease" ).mousedown(function (event) {
		// Left mouse button
		if(event.which === 1) {
			var newValue = $( ".manual .throttle .slider" ).slider("value");
			newValue -= 10;
			
			$( ".manual .throttle .slider" ).slider("value", newValue);
		}
	});
	
	// Increase throttle using button
	$( ".manual .throttle .increase" ).mousedown(function (event) {
		// Left mouse button
		if(event.which === 1) {
			var newValue = $( ".manual .throttle .slider" ).slider("value");
			newValue += 10;
			
			$( ".manual .throttle .slider" ).slider("value", newValue);
		}
	});
	
	
	
	// Change FL-angle using the slider control
	$( ".manual .fl-angle .slider" ).on("slidechange", function( event, ui ) {
		$( ".manual .fl-angle .value" ).text(ui.value);
		$.ajax("/cgi-bin/command.cgi?commandName=setAngle&wheel=FL&angle=" + ui.value);
	});
	
	// Set throttle to zero using button
	$( ".manual .fl-angle .buttonset .0" ).mousedown(function(event) {
		// Left mouse button
		if(event.which === 1) {
			$( ".manual .fl-angle .slider" ).slider("value", 0);
		}
	});
	
	// Decrease throttle using button
	$( ".manual .fl-angle .buttonset .decrease" ).mousedown(function (event) {
		// Left mouse button
		if(event.which === 1) {
			var newValue = $( ".manual .fl-angle .slider" ).slider("value");
			newValue -= 45;
			
			$( ".manual .fl-angle .slider" ).slider("value", newValue);
		}
	});
	
	// Increase throttle using button
	$( ".manual .fl-angle .increase" ).mousedown(function (event) {
		// Left mouse button
		if(event.which === 1) {
			var newValue = $( ".manual .fl-angle .slider" ).slider("value");
			newValue += 45;
			
			$( ".manual .fl-angle .slider" ).slider("value", newValue);
		}
	});	
});


// Set width of sliders based on width of existing elements
function sizeSliders() {
	var width = $(".manual .throttle").width();
	width -= 340;
	width = width + "px";
	
	$(".manual .throttle .slider").css("width", width);
	$(".manual .fl-angle .slider").css("width", width);
}

$(window).on("load", sizeSliders);

$(window).on("resize", function() {
	waitForFinalEvent(sizeSliders, 100, "unique1");
});
