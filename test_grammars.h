// Adding in this test grammar just to beta it. Will be removed in the final.
// test_grammar is purely for testing and was quickly thrown together to test
//   a few very specific functions.
// hh_grammar is a conversion of an existing group of messages from the mud

mapping test_grammar = ([
  "origin": ({
    "#sentence_start#, [badman:#bad_official.capitalizeAll.a#]#badman# was "+
      "#action#. Little did anyone know, #corruption# #consequence_good#. "+
      "#badman.capitalize# learned that crime pays."
    ,"#sentence_start#, #consequence_bad# "+
      "[badman:#bad_official.capitalizeAll.a#]#badman# #action#. No more "+
      "#fruit.s# for #badman#."
  }),
  "fruit": ({"apple", "banana", "coconut"}),
  "sentence_start": ({"#setting_start#","#time_start#"}),
  "setting_start": ({
    "In the heart of the city",
    "Behind closed doors at the government office",
    "In the shadowy halls of the ministry",
    "Within the walls of the bureaucracy"
  }),
  "time_start": ({
    "Seemingly burning the midnight oil",
    "By the dark of night",
    "On the weekend, free from prying eyes",
    "In broad daylight",
    "During a leisurely lunch",
    "While enjoying another state dinner",
    "At second breakfast"
  }),
  "bad_adjective": ({
    "corrupt",
    "slimy",
    "sly",
    "crooked",
    "cunning",
    "shrewd",
    "craven",
    "money-hungry"
  }),
  "bad_official": ({
    "#bad_adjective# #bureaucrat#"
  }),
  "bureaucrat": ({
    "official",
    "minister",
    "representative",
    "bureaucrat",
    "administrator",
    "councillor",
    "moff",
    "governor",
    "senator",
    "ambassador",
    "diplomat",
    "clerk"
  }),
  "action": ({
    "signing another suspicious contract",
    "approving funds for a nonexistent project",
    "meeting with a shady lobbyist",
    "stashing away bribes in offshore accounts",
    "diverting public funds for personal gain",
    "making backroom deals with powerful elites"
  }),
  "corruption": ({
    "the plan was falling apart",
    "the media was closing in",
    "the public was becoming aware",
    "the law was beginning to catch up",
    "a whistleblower was gathering evidence",
    "rumors of their dealings were spreading"
  }),
  "consequence_good": ({
    "but they covered their tracks expertly",
    "but they managed to bribe the investigators",
    "but a scapegoat was already in place",
    "but no one dared to confront them",
    "yet the system continued to protect them",
    "though their power only grew stronger"
  }),
  "consequence_bad": ({
    "lady justice peeled back her blindfold to catch",
    "#cops# finally discovered",
    "#cops# arrested",
    "#cops# were able to corroborate stories about",
  }),
  "cops": ({
    "investigators",
    "inquisitors",
    "authorities",
    "police",
    "the constabulary",
  })
]);

mapping hh_grammar = ([
  "band": ({ "Bartru and the Blaster Rays", "Danny and the Droids",
    "Figrin D'an and the Modal Nodes", "Geedo Loken and the Lo-keys",
    "Hakko Drazlip and the Tootle Froots", "Hyperspaze and the Jump Lanes",
    "Black Sabacc", "Gormo and the Grackers", "Grand Imperial Quartet",
    "Leisurenauts", "Minlan Weil and the Tam-honil Three", "Max Rebo Band",
    "Paradise Planet", "Proton Overload", "Raz and the Ubo-Kings", 
    "Senators of Rhythm", "Shag Kava", "South Sector Boppers",
    "Telindel and Saerlock", "Tulla Gladeyes and the Shrieking Squeebees",
    "Twisted Rancor Trio",
  }),
  "color": ({
    "red", "orange", "yellow", "green", "blue", "indigo", "violet", "grey",
    "brown", "black", "white", "pink", "silver", "gold", "copper", "cobalt",
    "periwinkle", "puce", "heliotrope", "super-intelligent-shade-of-blue",  
    "mauve", 
  }), 
  "ship": ({
    "shuttle", "frigate", "freighter", "cargo-shuttle", "pleasure yacht",
    "junker", "planet-hopper", "rinky-dinky fighter", "star-cruiser",
    "stunt fighter", "dreadnaught",
  }),
  "species": ({ 
    "Hutt", "Chadra'fan", "Bith", "Muun", "Tholothian", "Elomin", "Talz", 
    "Squib", "Arcona", "Nikto", "Vogon", "Weequay", "Kaminoan", "Ranats"   
  }),
  "shipSetup": "[shipColor:#color#][thisShip:#ship#]",
  "pilotSetup": "[pilotSpecies:#species#]#pilotGenderSetup#",
  "pilotGenderSetup": ({ 
    "[pilotGender:enby][pilotThey:they][pilotThem:them][pilotTheir:their]"+
      "[pilotTheirs:theirs]",
    "[pilotGender:guy][pilotThey:he][pilotThem:him][pilotTheir:his]"+
      "[pilotTheirs:his]",
    "[pilotGender:gal][pilotThey:she][pilotThem:her][pilotTheir:hers]"+
      "[pilotTheirs:hers]",
  }),
  "dest": ({
    "Af'El", "Alderaan", "Berchest", "Bespin", "Bimmisaari", "Borleias", "Byss",
    "Carida", "Centerpoint Station", "Corellia", "Corellia Defense Platform",
    "Coruscant", "Coruscant Station", "Crseih Station", "Da Soocha", "Dagobah",
    "Dantooine", "Eichbaum", "Endor", "Gamorr", "Honoghr", "Hoth", "Kashyyyk",
    "Kessel", "Korriban", "Kwenn Station", "Myrkr", "Naboo", "Nar Shaddaa",
    "Ord Mantell", "Roche", "Rodia", "Ryloth", "Sluis", "Sluis Van Shipyards",
    "Soleada", "Sullust", "Tatooine", "the Death Star", "the Lucrehulk",
    "the Super Star Destroyer", "Togoria", "Wayland", "Yavin",
  }),
  "verbSetupSingular": "[fly:flies][goose:gooses][launch:launches]",
  "pickup": ({
    "A #pilotSpecies# in #color.a# #ship# picks up your call and says "+
      "#pilotThey# can give you a lift as far as #dest#.",
    "You look overhead and see #thisShip.a# hover above you and drop a rope "+
      "ladder. You climb up and buckle in.",
    "As you're about to give up #pilotSpecies.a# in #shipColor.a# "+
      "#thisShip# across the docking area waves you over to #pilotTheir# "+
      "#thisShip#.",
    "A streak of #shipColor# fills your vision as #thisShip.a# almost lands "+
      "on top of you.",
    "A Vogon constructor ship beams you on board.",
    "A crazy #pilotSpecies# sends you a signal saying something about a "+
      "pulling 'crazy Ivan'. Before you figure out what's going on "+
      "#pilotThey# #fly# #color.a# #ship# backwards toward you. You jump "+
      "aboard right before they #goose# the engines. #pilotThey# "+
      "#launch# while the doors are still closing and blasts into space.",
    "Like a scene pulled straight from #band.a# album cover, #color# lights "+
      "shine down upon you as a tractor beam pulls you and anything loose "+
      "nearby into the #ship# as the hand of #pilotSpecies.a# #pilotGender# "+
      "reaches out to pull you safely inside.",
  }),
  "origin": "#pilotSetup##shipSetup##pickup#",
]);
