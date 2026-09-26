#ifndef SPACE_BODIES_H
#define SPACE_BODIES_H

typedef struct { const char *label, *value, *unit; } Datum;
typedef struct { const char *name; float radius, distance, speed; unsigned color; } Moon;
typedef struct { const char *name,*type,*chapter,*epigraph,*prose; Datum data[6]; float radius,dot_size,distance,orbit_speed,rotation_speed,galaxy_size; unsigned color; int rings,moon_count; Moon moons[4]; const char *galaxy_type; } Body;

#define BODY_COUNT 20
static const Body bodies[BODY_COUNT] = {
  { "Mercury", "planet", "Chapter I · The Messenger", "Closest to the fire. Farthest from mercy.", "I've always been fascinated by Mercury. No atmosphere to speak of, just rock and silence and a sun that looms three times larger than the one back home. The ground is pocked with craters, each one a scar from four billion years of impacts. Mercury doesn't care if you're there. It never has.", {
      {"Diameter","4,879","km"},
      {"Year","88","days"},
      {"Day","59","earth days"},
      {"Moons","0",""},
      {"Surface","−180 to 430","°C"},
      {"From Sol","57.9","M km"}
    }, 0.8f, 4.0f, 12.0f, 0.004f, 0.002f, 0.0f, 0x8a7f70, 0, 0, {{"",0,0,0,0},{"",0,0,0,0},{"",0,0,0,0},{"",0,0,0,0}}, "" },
  { "Venus", "planet", "Chapter II · The Twin", "Beautiful from afar. A furnace up close.", "Venus is the planet that broke my heart. The air is ninety six percent carbon dioxide. The pressure would crush you like a tin can. It rains sulfuric acid that evaporates before it hits the ground. And she spins backwards, as if she's ashamed of what she's become. A day there lasts longer than a year.", {
      {"Diameter","12,104","km"},
      {"Year","225","days"},
      {"Day","243","earth days"},
      {"Moons","0",""},
      {"Surface","462","°C"},
      {"From Sol","108.2","M km"}
    }, 1.2f, 6.0f, 18.0f, 0.0015f, -0.001f, 0.0f, 0xe8c890, 0, 0, {{"",0,0,0,0},{"",0,0,0,0},{"",0,0,0,0},{"",0,0,0,0}}, "" },
  { "Earth", "planet", "Chapter III · Home", "The only place we know of where the universe looked back.", "This is the one I keep coming back to. Where I felt rain on my face and watched the moon rise over the ocean. The water, the iron in my blood, the calcium in my bones. All of it forged in stars that died before our sun was born. Seven continents, five oceans, one thin skin of air holding everything together. A pale blue dot, suspended in a sunbeam. Home.", {
      {"Diameter","12,742","km"},
      {"Year","365.25","days"},
      {"Day","24","hours"},
      {"Moons","1",""},
      {"Surface","15","°C avg"},
      {"From Sol","149.6","M km"}
    }, 1.3f, 7.0f, 26.0f, 0.001f, 0.005f, 0.0f, 0x4a90e2, 0, 1, {{"Moon",0.35f,3.5f,0.02f,0xc0c0c0},{"",0,0,0,0},{"",0,0,0,0},{"",0,0,0,0}}, "" },
  { "Mars", "planet", "Chapter IV · The Frontier", "Once warm. Once wet. Now, waiting.", "Mars is where I go when I need to remember that things change. Olympus Mons rises twenty one kilometers above the plain. Valles Marineris splits the planet like a wound. Somewhere beneath that rust colored surface, water may still flow. The polar ice caps hold enough frozen water to cover the world eleven meters deep, if only it would melt.", {
      {"Diameter","6,779","km"},
      {"Year","687","days"},
      {"Day","24.6","hours"},
      {"Moons","2",""},
      {"Surface","−63","°C avg"},
      {"From Sol","227.9","M km"}
    }, 0.9f, 5.0f, 34.0f, 0.0008f, 0.005f, 0.0f, 0xc1440e, 0, 2, {{"Phobos",0.08f,2.2f,0.05f,0x8b7355},{"Deimos",0.06f,3.0f,0.03f,0x8b7355},{"",0,0,0,0},{"",0,0,0,0}}, "" },
  { "Jupiter", "planet", "Chapter V · The King", "A failed star, spinning in gas and storm.", "Thirteen hundred Earths could fit inside. The Great Red Spot is a hurricane older than any human civilization. Jupiter is less a planet than a small, failed sun, guarding the inner worlds from asteroids with its gravity. Its moon Europa may harbor life beneath a shell of ice.", {
      {"Diameter","139,820","km"},
      {"Year","11.9","earth years"},
      {"Day","9.9","hours"},
      {"Moons","95",""},
      {"Surface","−110","°C clouds"},
      {"From Sol","778.5","M km"}
    }, 2.8f, 14.0f, 52.0f, 0.0004f, 0.01f, 0.0f, 0xd8a878, 0, 4, {{"Io",0.28f,4.5f,0.04f,0xffff99},{"Europa",0.25f,5.8f,0.03f,0xe8e8e8},{"Ganymede",0.41f,7.2f,0.02f,0xc0c0c0},{"Callisto",0.38f,9.0f,0.015f,0x8b7355}}, "" },
  { "Saturn", "planet", "Chapter VI · The Jewel", "The answer to what if planets could be beautiful.", "Saturn is why I fell in love with the sky in the first place. Rings like spun glass, catching the light in a way that made my breath catch. They're billions of shards of ice and rock, each one a tiny moon. Saturn is so light it would float on water, if you could find an ocean large enough.", {
      {"Diameter","116,460","km"},
      {"Year","29.5","earth years"},
      {"Day","10.7","hours"},
      {"Moons","146",""},
      {"Rings","282,000","km wide"},
      {"From Sol","1.43","B km"}
    }, 2.4f, 12.0f, 68.0f, 0.0002f, 0.009f, 0.0f, 0xe8d8a0, 1, 2, {{"Titan",0.4f,6.0f,0.01f,0xd4a574},{"Enceladus",0.08f,3.5f,0.03f,0xffffff},{"",0,0,0,0},{"",0,0,0,0}}, "" },
  { "Uranus", "planet", "Chapter VII · The Tilted One", "Rolling on its side through the cold.", "Something hit it long ago. Hard enough to knock it onto its side. Now it rolls through its orbit like a ball. Pale, quiet, methane blue. The coldest planetary atmosphere in the system, colder than Neptune even though Neptune is farther from the sun. Thirteen faint rings circle it, discovered only in 1977.", {
      {"Diameter","50,724","km"},
      {"Year","84","earth years"},
      {"Day","17.2","hours"},
      {"Moons","27",""},
      {"Surface","−224","°C"},
      {"From Sol","2.87","B km"}
    }, 1.8f, 9.0f, 86.0f, 0.0001f, -0.007f, 0.0f, 0x9fd8e0, 0, 2, {{"Titania",0.12f,4.0f,0.015f,0xc0c0c0},{"Oberon",0.12f,5.0f,0.012f,0x8b7355},{"",0,0,0,0},{"",0,0,0,0}}, "" },
  { "Neptune", "planet", "Chapter VIII · The Last Blue", "Where the sun is just another star.", "The farthest out. Winds here reach two thousand one hundred kilometers per hour, faster than sound. It takes one hundred and sixty five years to complete one orbit. Its moon Triton orbits backwards, as if it were captured and never quite forgiven. Out here, the sun is just another star.", {
      {"Diameter","49,244","km"},
      {"Year","165","earth years"},
      {"Day","16.1","hours"},
      {"Moons","14",""},
      {"Winds","2,100","km/h"},
      {"From Sol","4.50","B km"}
    }, 1.8f, 9.0f, 102.0f, 0.00006f, 0.006f, 0.0f, 0x4166f5, 0, 1, {{"Triton",0.21f,4.5f,-0.01f,0xc0c0c0},{"",0,0,0,0},{"",0,0,0,0},{"",0,0,0,0}}, "" },
  { "Ceres", "dwarf", "Appendix I · The Belt Queen", "Hidden in plain sight among the rubble.", "Ceres is the largest object in the asteroid belt, a lonely world of ice and rock caught between Mars and Jupiter. For centuries it was classified as an asteroid, then promoted to dwarf planet in 2006. It has bright spots of sodium carbonate on its surface, remnants of a subsurface ocean that may still exist deep below.", {
      {"Diameter","940","km"},
      {"Year","1,682","days"},
      {"Day","9.1","hours"},
      {"Moons","0",""},
      {"Surface","−105","°C avg"},
      {"From Sol","414","M km"}
    }, 0.35f, 3.0f, 44.0f, 0.0006f, 0.004f, 0.0f, 0x8a8a8a, 0, 0, {{"",0,0,0,0},{"",0,0,0,0},{"",0,0,0,0},{"",0,0,0,0}}, "" },
  { "Pluto", "dwarf", "Appendix II · The Exile", "Demoted but never forgotten.", "Pluto was the ninth planet of my childhood, and its demotion in 2006 still stings. But seeing the images from New Horizons in 2015 changed everything. A heart shaped glacier of nitrogen ice. Mountains of water ice. A thin blue atmosphere. Pluto is not a failed planet. It is a world in its own right.", {
      {"Diameter","2,377","km"},
      {"Year","248","earth years"},
      {"Day","6.4","earth days"},
      {"Moons","5",""},
      {"Surface","−230","°C avg"},
      {"From Sol","5.9","B km"}
    }, 0.5f, 4.0f, 115.0f, 0.00004f, -0.003f, 0.0f, 0xc4a882, 0, 1, {{"Charon",0.25f,2.5f,0.015f,0xa0a0a0},{"",0,0,0,0},{"",0,0,0,0},{"",0,0,0,0}}, "" },
  { "Eris", "dwarf", "Appendix III · The Troublemaker", "The one that started the argument.", "Eris is the reason Pluto lost its planet status. Discovered in 2005, it is slightly more massive than Pluto and orbits even farther out. Named after the Greek goddess of discord, which feels appropriate given the controversy it caused. Its surface is covered in methane ice, making it one of the most reflective objects in the solar system.", {
      {"Diameter","2,326","km"},
      {"Year","559","earth years"},
      {"Day","25.9","hours"},
      {"Moons","1",""},
      {"Surface","−243","°C avg"},
      {"From Sol","10.2","B km"}
    }, 0.48f, 3.0f, 130.0f, 0.000025f, 0.002f, 0.0f, 0xd0d0d0, 0, 1, {{"Dysnomia",0.1f,2.0f,0.01f,0x888888},{"",0,0,0,0},{"",0,0,0,0},{"",0,0,0,0}}, "" },
  { "Haumea", "dwarf", "Appendix IV · The Spinner", "Stretched thin by its own fury.", "Haumea spins so fast that it has been stretched into an ellipsoid, looking more like a rugby ball than a sphere. One rotation takes less than four hours. It has a ring system, the first dwarf planet known to have one, and two small moons named after the daughters of the Hawaiian goddess it was named for.", {
      {"Diameter","1,632","km"},
      {"Year","283","earth years"},
      {"Day","3.9","hours"},
      {"Moons","2",""},
      {"Surface","−241","°C avg"},
      {"From Sol","6.5","B km"}
    }, 0.42f, 3.0f, 122.0f, 0.000035f, 0.02f, 0.0f, 0xc8b8a8, 0, 2, {{"Hiiaka",0.08f,2.0f,0.02f,0xaaaaaa},{"Namaka",0.05f,1.5f,0.03f,0x999999},{"",0,0,0,0},{"",0,0,0,0}}, "" },
  { "Makemake", "dwarf", "Appendix V · The Silent One", "No atmosphere. No sound. Just ice and void.", "Makemake is one of the quietest worlds we know of. No significant atmosphere, no known geological activity, just a surface of methane and ethane ice reflecting the distant sun. It is reddish brown, similar to Pluto, but smaller and lonelier. Named after the Rapa Nui god of fertility.", {
      {"Diameter","1,430","km"},
      {"Year","306","earth years"},
      {"Day","22.8","hours"},
      {"Moons","1",""},
      {"Surface","−240","°C avg"},
      {"From Sol","6.8","B km"}
    }, 0.4f, 3.0f, 125.0f, 0.00003f, 0.003f, 0.0f, 0xb07050, 0, 1, {{"MK2",0.06f,1.8f,0.015f,0x777777},{"",0,0,0,0},{"",0,0,0,0},{"",0,0,0,0}}, "" },
  { "Milky Way", "homegalaxy", "Home · Our Galaxy", "A hundred thousand light years across, and we are somewhere in it.", "The Milky Way is home. A barred spiral galaxy containing between one hundred and four hundred billion stars, one of which is our sun. We live in a minor spiral arm called the Orion Arm, about twenty six thousand light years from the galactic center. From a dark sky, the galaxy appears as a faint band of light stretching across the heavens. It is the sum of all those distant suns, blended into a single milky river. We are inside it, looking out.", {
      {"Type","Barred Spiral",""},
      {"Diameter","100,000","ly"},
      {"Stars","100 to 400B",""},
      {"Age","13.6","billion years"},
      {"Our position","Orion Arm",""},
      {"From center","26,000","ly"}
    }, 0.0f, 10.0f, 0.0f, 0.0f, 0.00008f, 800.0f, 0xf0e8d0, 0, 0, {{"",0,0,0,0},{"",0,0,0,0},{"",0,0,0,0},{"",0,0,0,0}}, "milkyway" },
  { "Andromeda", "galaxy", "Beyond I · The Neighbor", "Two and a half million light years away, and closing.", "Andromeda is the nearest major galaxy to our own Milky Way, a sprawling spiral of a trillion stars visible to the naked eye as a faint smudge in the autumn sky. It is larger than our galaxy and approaching us at 110 kilometers per second. In about four and a half billion years, the two galaxies will collide and merge.", {
      {"Type","Spiral",""},
      {"Distance","2.537","M ly"},
      {"Diameter","220,000","ly"},
      {"Stars","1 trillion",""},
      {"Mass","1.2T","solar masses"},
      {"Approaching","110","km/s"}
    }, 0.0f, 6.0f, 0.0f, 0.0f, 0.0003f, 80.0f, 0xc8b8ff, 0, 0, {{"",0,0,0,0},{"",0,0,0,0},{"",0,0,0,0},{"",0,0,0,0}}, "spiral" },
  { "Triangulum", "galaxy", "Beyond II · The Pinwheel", "The third wheel of the Local Group.", "Triangulum is the third largest galaxy in our Local Group, a face on spiral that looks like a pinwheel frozen in time. It is small compared to Andromeda and the Milky Way, containing only about forty billion stars. But it is one of the most distant objects visible to the naked eye.", {
      {"Type","Spiral",""},
      {"Distance","2.73","M ly"},
      {"Diameter","60,000","ly"},
      {"Stars","40 billion",""},
      {"Mass","50B","solar masses"},
      {"Also known as","M33",""}
    }, 0.0f, 4.0f, 0.0f, 0.0f, 0.0004f, 45.0f, 0xa0c0ff, 0, 0, {{"",0,0,0,0},{"",0,0,0,0},{"",0,0,0,0},{"",0,0,0,0}}, "spiral" },
  { "Large Magellanic Cloud", "galaxy", "Beyond III · The Companion", "A satellite galaxy, tethered by gravity.", "The Large Magellanic Cloud is a satellite galaxy of the Milky Way, orbiting us at a distance of 160,000 light years. It is an irregular galaxy, lacking the elegant spiral structure of Andromeda, but it contains the Tarantula Nebula, the most active star forming region in the entire Local Group.", {
      {"Type","Irregular",""},
      {"Distance","160,000","ly"},
      {"Diameter","14,000","ly"},
      {"Stars","30 billion",""},
      {"Mass","10B","solar masses"},
      {"Also known as","LMC",""}
    }, 0.0f, 5.0f, 0.0f, 0.0f, 0.0002f, 30.0f, 0xffe0a0, 0, 0, {{"",0,0,0,0},{"",0,0,0,0},{"",0,0,0,0},{"",0,0,0,0}}, "irregular" },
  { "Small Magellanic Cloud", "galaxy", "Beyond IV · The Little Sister", "Smaller, fainter, but no less ancient.", "The Small Magellanic Cloud is the smaller companion of the Large Magellanic Cloud, a dwarf irregular galaxy about 200,000 light years away. It is being slowly torn apart by the gravitational pull of the Milky Way, its stars streaming out in a long tidal tail.", {
      {"Type","Irregular",""},
      {"Distance","200,000","ly"},
      {"Diameter","7,000","ly"},
      {"Stars","3 billion",""},
      {"Mass","7B","solar masses"},
      {"Also known as","SMC",""}
    }, 0.0f, 3.0f, 0.0f, 0.0f, 0.0003f, 18.0f, 0xd0c8b0, 0, 0, {{"",0,0,0,0},{"",0,0,0,0},{"",0,0,0,0},{"",0,0,0,0}}, "irregular" },
  { "Sombrero Galaxy", "galaxy", "Beyond V · The Hat", "A bright core wrapped in dust.", "The Sombrero Galaxy gets its name from its appearance: a bright central bulge surrounded by a wide, dark dust lane that makes it look like a Mexican hat seen from the side. It sits about thirty million light years away. Its central black hole is one billion solar masses.", {
      {"Type","Edge on Spiral",""},
      {"Distance","29.6","M ly"},
      {"Diameter","49,000","ly"},
      {"Stars","100 billion",""},
      {"Black hole","1B","solar masses"},
      {"Also known as","M104",""}
    }, 0.0f, 4.0f, 0.0f, 0.0f, 0.0002f, 28.0f, 0xffe8c0, 0, 0, {{"",0,0,0,0},{"",0,0,0,0},{"",0,0,0,0},{"",0,0,0,0}}, "edgeon" },
  { "Whirlpool Galaxy", "galaxy", "Beyond VI · The Spiral", "The galaxy that proved spirals exist.", "The Whirlpool Galaxy was the first galaxy in which spiral structure was observed, by Lord Rosse in 1845. It is a grand design spiral, its arms sweeping out in elegant curves studded with pink star forming regions. It is interacting with a smaller companion galaxy.", {
      {"Type","Grand Design Spiral",""},
      {"Distance","23","M ly"},
      {"Diameter","76,000","ly"},
      {"Stars","160 billion",""},
      {"Mass","160B","solar masses"},
      {"Also known as","M51",""}
    }, 0.0f, 5.0f, 0.0f, 0.0f, 0.0003f, 48.0f, 0xc0d0ff, 0, 0, {{"",0,0,0,0},{"",0,0,0,0},{"",0,0,0,0},{"",0,0,0,0}}, "spiral" },
};
#endif
