var address = "192.168.43.236";
var device = new Device(address);

//record the current movement to prevent spamming of same direction on key hold.
var currentMovement = "";


//  KEYBOARD CONTROLS
window.addEventListener("keydown", onKeyDown, false);
window.addEventListener("keyup", onKeyUp, false);

function onKeyDown(event) {
  var keyCode = event.keyCode;
  switch (keyCode) {
    case 68: //d
    if (currentMovement != "right"){
      //if 'd' is held - the command will only be send once
      callDeviceFunction("right");
      currentMovement = "right";
    }
      break;
    case 83: //s
		if (currentMovement != "backward"){
      callDeviceFunction("backward");
      currentMovement = "backward";
    }
      break;
    case 65: //a
    if (currentMovement != "left"){
      callDeviceFunction("left");
      currentMovement = "left";
    }
      break;
    case 87: //w
    console.log("w down");
		if (currentMovement != "forward"){
      callDeviceFunction("forward");
      currentMovement = "forward";
    }
      break;
  }
}

function onKeyUp(event) {
  // var keyCode = event.keyCode;

  // //to p
  // switch (keyCode) {
  //   case 68: //d
  //   callStop();
  //     break;
  //   case 83: //s
  //   callStop();
  //     break;
  //   case 65: //a
  //   callStop();
  //     break;
  //   case 87: //w
  //     callStop();
  //     break;
  // }

  callDeviceFunction("stop");
}


//    SCREEN UI
$('#forward').mousedown(function() {
	callDeviceFunction("forward");
});
$('#forward').mouseup(function() {
  callDeviceFunction("stop");
});

$('#backward').mousedown(function() {
  callDeviceFunction("backward");
});
$('#backward').mouseup(function() {
  callDeviceFunction("stop");
});

$('#left').mousedown(function() {
  callDeviceFunction("left");
});
$('#left').mouseup(function() {
  callDeviceFunction("stop");
});

$('#right').mousedown(function() {
  callDeviceFunction("right");
});
$('#right').mouseup(function() {
  callDeviceFunction("stop");
});

//	SCREEN UI (speed)
$('#increaseSpeed').mousedown(function() {
  callDeviceFunction("increaseSpeed");
});
$('#decreaseSpeed').mousedown(function() {
  callDeviceFunction("decreaseSpeed");
});


//	 Helper Functions
function callDeviceFunction(command){
  device.callFunction(command);
  currentMovement = command;
}


