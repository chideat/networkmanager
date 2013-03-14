function drawer() {
	var c = document.getElementById("drawer");
	var cxt = c.getContext("2d");
	var grd=cxt.createLinearGradient(0,0,300,150);
	grd.addColorStop(0,"#FF0000");
	grd.addColorStop(1,"#00FF00");
	cxt.fillStyle=grd;
	cxt.fillRect(0,0,300,150);
}



