setInterval(function(){
	var captures = Math.floor(Math.random() * 10) + 1;
	var color = 'blue';
	var pixels = 0;

	if (captures >= 10) {
		color = '#00E676';
		pixels = 0;
	}
	else if(captures< 10 && captures >= 8){
		color = '#00E650';
		pixels = 40;
	}
	else if(captures < 8 && captures >= 6){
		color = '#81C784';
		pixels = 80;
	}
	else if (captures < 6 && captures >= 4){
		color = '#FFEB3B';
		pixels = 120;
	}
	else if (captures < 4 && captures >= 2){
		color = '#FF9800';
		pixels = 160;
	}
	else if (captures < 2 && captures >= 0){
		color = '#FF3D00';
		pixels = 185;
	}

	$('.column').css({background: color});
	
	$('.column').animate({
		height: captures * 10 + '%',
	});

	$('.num').css({'padding-top': pixels + 'px'});

	$('.num').animate({'padding-top': pixels + 'px'});

	document.getElementById("app").querySelector(".num").textContent = captures;


}, 4000);

