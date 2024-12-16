
# CookingChords

**CookingChords** is a VR rhythm game that merges the excitement of rhythm-based gameplay with the creativity of cooking. In this immersive experience, players slice, dice, and prepare virtual ingredients to the beat of music, turning the kitchen into a stage for their culinary and rhythmic talents.

## Project Vision

The goal of CookingChords is to create a unique VR experience where rhythm meets cooking. By integrating music with interactive cooking mechanics, players can enjoy a dynamic and engaging game that sharpens their sense of timing and creativity. From slicing fruits to shooting different dressings on approaching slices of bread, the game promises a blend of rhythm-based challenges and culinary fun.

## Features

- **Dynamic Slicing:** Players can slice objects (ingredients) in sync with the beat of a song, using VR controllers as virtual knives.
- **Level Editor:** A fully functional level editor allows players to upload their own music, visualize its waveform (work in progress), and create a working level with all the interesting mechanics the game has to offer.
- **Runtime Audio Import:** Players can upload `.wav` files at runtime, integrating their favorite tracks into the game.

## Challenges & Struggles

Building CookingChords has been a rewarding journey, but not without its struggles. Here are the key hurdles we've faced so far:

### 1. **Adjusting to Unreal Engine**

Transitioning to Unreal Engine for this project was both exciting and daunting. Understanding the nuances of Unreal’s C++ API, Blueprints, and the way components interact required significant effort. From learning how to properly initialize widgets to managing actor lifecycles, every step was a learning experience.

### 2. **Unfamiliar Functions and Usage**

Unreal Engine offers an abundance of powerful features, but understanding how to use them effectively was a steep learning curve. For example, implementing level transitions and ensuring proper player placement took several iterations before achieving the desired results. Navigating documentation, debugging errors, and experimenting with various approaches became part of the daily grind.

### 3. **Runtime Music Import**

One of the most ambitious features of CookingChords is the ability to import music at runtime. This feature turned out to be far more challenging than anticipated. Initially, importing `.wav` files and processing their data seemed straightforward, but we quickly ran into issues with data conversion, audio component initialization, and memory management. After days of trial and error, we finally managed to get the music importing and playing correctly.

### 4. **Waveform Visualization**

Displaying the song’s waveform on the slider remains a work in progress. While we’ve managed to generate amplitude data from the audio, translating this data into a visually appealing and functional waveform has proven tricky. Issues include ensuring the waveform matches the slider dimensions and creating a responsive design that adapts to different songs.

### 5. **Time Management and External Challenges**

Balancing time for the project amidst other responsibilities was initially difficult. However, as the development progressed, we improved significantly in planning and executing tasks more efficiently. We learned that attempting to tackle big tasks as a whole is a terrible idea. A big task can feel daunting, causing the human brain to become distracted and avoid the hard work at all costs. Instead, it’s crucial for a good developer to be able to break the task down into smaller, actionable problems. This approach not only makes the work more manageable but also provides a clear path to completion. Additionally, this project was developed during a time of war in Israel, which presented unique mental and tangible challenges. Despite these hardships, the project provided a much-needed focus and sense of purpose during trying times.

## Leveraging Academic Knowledge

This project provided a fantastic opportunity to apply concepts learned throughout my degree. From storing custom level data in a hashmap to splitting tasks into modular functions, I implemented practices that improved the project’s structure and maintainability. Additionally, focusing on code readability through documentation and adhering to consistent naming conventions has ensured that the codebase remains clear and scalable. The project was also managed using GitHub, which allowed for effective version control and collaboration.

## Future Plans

- **Waveform Display:** Complete the implementation of the waveform visualization for uploaded songs.
- **Expanded Cooking Mechanics:** Add new cooking-related tasks to make gameplay more unique and interesting.
- **Enhanced Level Editor:** Allow for more customization options, including object placement and unique challenge configurations.

## Acknowledgments

Special thanks to my instructor Roi Poranne, who has been there to advise and help out when I needed guidance, even considering the unique circumstances under which the development of this project took place.

---

Building CookingChords has been a journey of learning and growth. While there’s still much to do, I'm proud of how far I've come.
