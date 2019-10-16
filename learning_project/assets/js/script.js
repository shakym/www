$(document).ready(function(){

    $(".subtitle").click(function(){
        $(".call-to-action").slideToggle();
    });
    
    $(".call-button").click(function(e){
        e.preventDefault();
        $(".column img").toggleClass("bigger")
    })
    
});