// Hook jquery-ui onto the appropriate elements
$(function() {
	$( ".global" ).accordion({
		heightStyle: "content"
	});
	
	$( "button" ).button();
	
	$( ".global .states" ).buttonset();
	
	
	var globalThrottleSlider = $( ".global .throttle .slider" )
	
	globalThrottleSlider.slider({
		value: 0,
		min: 0,
		max: 100,
		animate: "slow",
		change: function( event, ui ) {
			$( ".global .throttle .current-value" ).text(ui.value);
			$.ajax("/cgi-bin/command.cgi?commandName=setThrottle&throttle=" + ui.value);
		}
	});
	
	// Allow sliders to be controlled via the mouse-wheel
	globalThrottleSlider.mousewheel( function(event, delta) {
		globalThrottleSlider.slider("value", globalThrottleSlider.slider("value") + delta*2);
	});
	
	// Event listener for the global state radio buttons
	$( ".global .states input[type=radio]" ).change(function() {
		$.ajax("/cgi-bin/command.cgi?commandName=setState&state=" + this.value);
	});
});
