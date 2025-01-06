
# CookingChords

**CookingChords** is a VR rhythm game that I started working on as my final project for my bachelor's degree, it felt like a long time coming, since I got into this field due to my passion surrounding video games and how I practically grew up on them. This has been one of the greatest learning experiences for me, I would go as far as to say the most valuable thing I've done throughout my degree as a lot of things finally came around full circle for this project to be what it is.

## Project Vision

Although initially I was very excited to work on a game that isn't a VR rhythm game, we were required to stick to this theme, and although I had fun ideas (that I will likely come back to later on, on my own, after my degree), this felt like a game-jam, where we get a theme and we get a time slot to create a game within, mine was half a year, the goal was to make a cooking themed rhythm game, and I had brainstormed a lot of cooking related features, some of which I ended up scrapping, and other's made it to the game.

In all honesty I think half a year was enough time to deliver a better game than what I have today, however I am aware that I, personally, went through extreme, unique circumstances, alongside the complications we as a collective went through here in Israel living through a war.

 I am proud of myself and CookingChords regardless.

## Features

- **Game Mechanics:** Players can **Slice** ingredients, **Shoot** condiments on flying slices of bread, **Knead** bigger, more stubborn fruits, and **Mix** an incoming bowl in sync with the beat of a song, using VR controllers as virtual knives.
- **Level Editor:** A fully functional level editor allows players to upload their own music, visualize its waveform (work in progress), and create a working level.
- **Runtime Audio Import:** Players can upload `.wav` files at runtime, integrating their favorite tracks into the game.

## Challenges & Struggles

Building CookingChords has been a rewarding journey, but with a lot of ups and downs along the way..

The first hurdle I had faced was Unreal Engine itself, I think that of the two titans in the industry, Unreal is the more complicated one, with more things I had to wrap my brain around, this took some time, and truth be told felt extremely intimidating at first.
I decided to watch a few videos on youtube, but avoided "Tutorial Hell" well, and once I felt like I understood the slightest bit about Unreal Engine, I dived right in, and it was the best decision since I am a firm believer that Hands-On experience is the best teacher.

This made me quickly learn what the various functions and variables do, how to work with c++ and Blueprints alongside each other for the most efficient and creative solutions, and I am so happy that I can look back at where I was, for example when I desprately needed help from the internet or chat gpt for every little thing. And now look at where I am today, where for the entire latter half of the project I did everything on my own, no internet, no chat gpt, just me and my experience with the engine.

The biggest time sink that I hit was Runtime Music Import, at first I tried doing it by myself without using an existing plug-in, and it took me days, perhaps a week, to get anything working, eventually I had managed to get the song to play!!, I was so happy that day, only to find out the next day that I cant pause it or fast forward or do anything to the song, so I had to roll back my progress, and contacted my instructor who recommended a plug-in (which I had seen before but thought was paid and had previously decided to just do it myself), and after fidling with it for a while, managed to get it integrated and working properly.

The biggest ongoing problem however, is to visualize the waveform of the song thats uploaded, my goal is to have it as the background of the slider, so that the player can use the slider to hit certain big waves in the waveform for the most satisfying interactions at those time stamps. But this is proving to be difficult and I have this pinned to come back to later on.

My proudest challenge that I overcame was Time Management, I had struggled with it my entire life, and on a solo project it was more apparent that I had to work on myself, but I believe I managed to remedee my habits with a simple, effective fix, which I now know, is characteristic of great programmers, and project managers in general: Huge tasks are daunting, and your human brain does everything in its power to avoid the monster around the corner, your goal, to overcome this, should be to train yourself to see through the facade, the monster is not that scary, its actually made up of tiny little minions standing on top of each other to create that illusion, once you train yourself to be able to see the little minion components, it becomes easier for you to tackle one of them at a time, divide and conquer at its finest.

## Leveraging Academic Knowledge

In this section I will be talking about the "full-circle" I alluded to earlier..

When I took Data Structures in my degree, we were short on time towards the end of the semester and the lecturer only managed to discuss hashmaps with us in passing, which meant I never fully got it and was hesitant to study and practice it for the exam, meaning I had completed the course without having a good grasp on it and it's uses. Now, considering I store the created level in a Hashmap initially, which means I got to actually code it and use it practically, I finally see the merit and understand more closeley how and when it works.

More predominantly however, are OOP philosophies and concepts, that I got to implement, and felt the importance of, splitting tasks into smaller methods, when you have many objects that intend to do a similar thing at the core, inheritance comes in handy, seeing different classes interacting with each other, I feel more inline with the OOP approach more than ever before.


## Acknowledgments

Finally I'd like to thank my instructor Roi Poranne, who has been there to advise and help out when I needed guidance, even considering the unique circumstances under which the development of this project took place.

---

Building CookingChords has been a journey of learning and growth. While there's still much to do, I'm proud of how far I've come.
