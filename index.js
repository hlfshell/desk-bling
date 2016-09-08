require('dotenv').config();
const Slackbot = require('slackbots');
const bot = new Slackbot({
	token: process.env.SLACK_TOKEN,
	name: process.env.BOT_NAME,
	channels: "general"
});
var idToBlinkOn = process.env.REACT_ON_USER;
const Particle = require('particle-beam');
const lights = Particle({
	device: process.env.PHOTON_DEVICE_ID,
	token: process.env.PHOTON_TOKEN
});
const triggerWord = process.env.TRIGGER_WORD || "desk-bling";
const fs = require('fs');
const helpMessage = fs.readFileSync("./helpMessage").toString();

bot.on('start', function(){
	console.log("BLING-TRON SLACKBOT 3000 STARTED");
});

var handleResponse = function(){
	console.log("PHOTON RESPONSE", arguments);
}

bot.on('message', function(data){
	if(data.type == 'message' && data.text){

		if(data.text.indexOf("<@" + idToBlinkOn + ">") != -1){
			lights("blinkLights", handleResponse);
		} else if(data.text.substring(0, triggerWord.length) == triggerWord){
			if(data.text.split(" ").length <= 1) return;

			var command = data.text.toLowerCase().split(" ");
			command.shift();

			switch(command[0]){
				case "partytime":
					lights("partyMode", handleResponse);
					break;

				case "color":
					command.shift();
					if(command.length != 3 ||
						isNaN(parseInt(command[0])) ||
						isNaN(parseInt(command[1])) ||
						isNaN(parseInt(command[2])) ) return;

					lights("setColor", command.join(","), handleResponse);
					break;

				case "fade":
					command.shift();
					if(command.length < 3 ||
						command.length > 4 ||
						isNaN(parseInt(command[0])) ||
						isNaN(parseInt(command[1])) ||
						isNaN(parseInt(command[2])) ) return;
						
					//If they didn't add the time delay, go to our default
					if(command.length == 3) command.push("20");

					lights("fadeColor", command.join(","), handleResponse);
					break;

				case "pulse":
					command.shift();
					if(command.length < 6 ||
					   	command.length > 7 ||
						isNaN(parseInt(command[0])) ||
						isNaN(parseInt(command[1])) ||
						isNaN(parseInt(command[2])) ||
						isNaN(parseInt(command[3])) ||
						isNaN(parseInt(command[4])) ||
						isNaN(parseInt(command[5])) ) return;

					//If they didn't add the time delay, go to our default
					if(command.length == 6) command.push("20");

					lights("pulseColors", command.join(","), handleResponse);
					break;

				case "off":
					lights("setColor", "0,0,0", handleResponse);
					break;

				case "angry":
					lights("pulseColors", "255,0,0,0,0,0,20", handleResponse);
					break;

				case "calm":
					lights("pulseColors", "150,219,242,173,242,150,20", handleResponse);
					break;

				case "seizure":
					lights("pulseColors", "255,255,255,0,0,0,0", handleResponse);
					break;

				case "help":
					bot.getUsers().then(function(value){
						value.members.forEach(function(user){
							if(user.id == data.user)
								bot.postMessageToUser(user.name, helpMessage)
						});
					});
					break;
			}
		}
	}
});

//This may seem odd, but if you provided the ID, this won't trigger, leaving it.
//If you used the NAME instead, then this will find the id and insert it
bot.on("start", function(){
	bot.getUsers().then(function(data){
		data.members.forEach(function(user){
			if(user.name == idToBlinkOn){
				idToBlinkOn = user.id;
			}
		});
	});
});