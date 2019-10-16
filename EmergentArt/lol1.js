var nParticles = 100;
var particles = [];
var ParticleSize = 40;

function createParticle() {
  var particle = {};
  particle.position = createVector(
    Math.random() * width,
    Math.random() * height
  );
  particle.direction = createVector(
    Math.random(),
    Math.random()
  );
  particle.update = function(){
    this.position.add(this.direction)
    
  }
}


function setup() {
  createCanvas(400,400);
  background(200);
  stroke(0,80);
  fill(0,90);
  
}

function draw() {
  
}