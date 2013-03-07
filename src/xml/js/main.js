//functions define
function enableDevice(device) {
}

function load() {
    if(Q_Network.networkUp) {
        //allow showing device list
        $("#device-list").show();
        $("#network").addClass("enabled");
    }
    else 
    if(Q_Network.wirelessUp) {
        $("#device-list").show();
        $("#wireless").addClass("enabled");
    }
    else {
        $("#wireless").removeClass("enabled");
    }
    
}

//actions
$(document).ready(function(){
    $(".item").on("click",function() {
        $(this).find(".setting").show();
    });
    
    
    $(".item").on("mouseleave",function() {
        $(this).find(".setting").hide();
    });
    
    $("#popup").on("click", function() {
        $("#advanced").show();
    });
    
    $("#advanced").on("mouseleave", function() {
        $(this).hide();
    });
    
    $("#popdown").on("click", function(event) {
        $(this).parent().hide();
        event.stopPropagation();
    });
    
    $(".button").on("click", function(event){
        alert($(event.target).id);
        if($(event.target) === $("#network")) {
                Q_Network.enableNetwork(!$("#network").hasClass("enabled"));
        }
        else if($(event.target) === $("#wireless")) {
                Q_Network.enableWireless(!$("#wireless").hasClass("enabled"));
        }
    });
    
    //here connect javascript function to qt signals
    Q_Network.loadFinished.connect(load);
});
