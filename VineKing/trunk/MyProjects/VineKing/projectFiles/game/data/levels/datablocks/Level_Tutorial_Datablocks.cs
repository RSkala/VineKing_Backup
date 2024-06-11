$levelDatablocks = new SimSet() {
	canSaveDynamicFields = "1";
	setType = "Datablocks";
	
	new t2dImageMapDatablock(enemy_cell_00ImageMap) {
		imageName = "~/data/images/enemy_cell_00.png";
		imageMode = "CELL";
		frameCount = "-1";
		filterMode = "SMOOTH";
		filterPad = "0";
		preferPerf = "1";
		cellRowOrder = "1";
		cellOffsetX = "0";
		cellOffsetY = "0";
		cellStrideX = "0";
		cellStrideY = "0";
		cellCountX = "-1";
		cellCountY = "-1";
		cellWidth = "32";
		cellHeight = "64";
		preload = "0";
		allowUnload = "0";
		compressPVR = "0";
		optimised = "0";
		force16bit = "0";
	};
	new t2dImageMapDatablock(lava_tile_cell_01ImageMap) {
		imageName = "~/data/images/lava_tile_cell_01.png";
		imageMode = "CELL";
		frameCount = "-1";
		filterMode = "NONE";
		filterPad = "0";
		preferPerf = "1";
		cellRowOrder = "1";
		cellOffsetX = "0";
		cellOffsetY = "0";
		cellStrideX = "0";
		cellStrideY = "0";
		cellCountX = "-1";
		cellCountY = "-1";
		cellWidth = "32";
		cellHeight = "64";
		preload = "0";
		allowUnload = "0";
		compressPVR = "0";
		optimised = "0";
		force16bit = "0";
	};
	new t2dImageMapDatablock(vineking_cells_000ImageMap) {
		imageName = "~/data/images/vineking_cells_000.png";
		imageMode = "CELL";
		frameCount = "-1";
		filterMode = "NONE";
		filterPad = "0";
		preferPerf = "1";
		cellRowOrder = "1";
		cellOffsetX = "0";
		cellOffsetY = "0";
		cellStrideX = "0";
		cellStrideY = "0";
		cellCountX = "-1";
		cellCountY = "-1";
		cellWidth = "64";
		cellHeight = "128";
		preload = "0";
		allowUnload = "0";
		compressPVR = "0";
		optimised = "0";
		force16bit = "0";
	};
	new t2dImageMapDatablock(frame_00ImageMap) {
		imageName = "~/data/images/frame_00.png";
		imageMode = "CELL";
		frameCount = "-1";
		filterMode = "NONE";
		filterPad = "0";
		preferPerf = "1";
		cellRowOrder = "1";
		cellOffsetX = "0";
		cellOffsetY = "0";
		cellStrideX = "0";
		cellStrideY = "0";
		cellCountX = "-1";
		cellCountY = "-1";
		cellWidth = "32";
		cellHeight = "32";
		preload = "0";
		allowUnload = "0";
		compressPVR = "0";
		optimised = "0";
		force16bit = "0";
	};
	new t2dAnimationDatablock(VINE_KING_Death) {
		imageMap = "vineking_cells_000ImageMap";
		animationFrames = "8 9 9 10 11 12 13 13";
		animationTime = "1";
		animationCycle = "0";
		randomStart = "0";
		startframe = "0";
		pingPong = "0";
		playForward = "1";
	};
	new t2dAnimationDatablock(vine_king_mana_drain) {
		imageMap = "vineking_cells_000ImageMap";
		animationFrames = "0 0 1 0 0 1";
		animationTime = "1";
		animationCycle = "0";
		randomStart = "0";
		startframe = "0";
		pingPong = "0";
		playForward = "1";
	};
	new t2dAnimationDatablock(VINE_KING_LINE_DRAW) {
		imageMap = "vineking_cells_000ImageMap";
		animationFrames = "2 2 3 4 4";
		animationTime = "0.625";
		animationCycle = "1";
		randomStart = "0";
		startframe = "0";
		pingPong = "0";
		playForward = "1";
	};
	new t2dAnimationDatablock(VINE_KING_EAT) {
		imageMap = "vineking_cells_000ImageMap";
		animationFrames = "5 6 7";
		animationTime = "0.3";
		animationCycle = "1";
		randomStart = "0";
		startframe = "0";
		pingPong = "0";
		playForward = "1";
	};
	new t2dAnimationDatablock(VINE_KING_ATTACK) {
		imageMap = "vineking_cells_000ImageMap";
		animationFrames = "14 15 15 15";
		animationTime = "0.125";
		animationCycle = "0";
		randomStart = "0";
		startframe = "0";
		pingPong = "0";
		playForward = "1";
	};
	new t2dAnimationDatablock(VINE_KING_WIN) {
		imageMap = "vineking_cells_000ImageMap";
		animationFrames = "24 25 26 26 27 29 30";
		animationTime = "0.7";
		animationCycle = "1";
		randomStart = "0";
		startframe = "0";
		pingPong = "0";
		playForward = "1";
	};
	new t2dAnimationDatablock(VINE_KING_IDLE) {
		imageMap = "vineking_cells_000ImageMap";
		animationFrames = "16 17 18 19 16 17 18 19 20 20 20 21 20 20 21 21 20 22 22 23 22 23 22 23 22 23 22 23";
		animationTime = "5.6";
		animationCycle = "1";
		randomStart = "0";
		startframe = "0";
		pingPong = "0";
		playForward = "1";
	};
	new t2dAnimationDatablock(SERPENT_SPAWN) {
		imageMap = "enemy_cell_00ImageMap";
		animationFrames = "64 65 65 66 68 69";
		animationTime = "0.428571";
		animationCycle = "0";
		randomStart = "0";
		startframe = "0";
		pingPong = "0";
		playForward = "1";
	};
	new t2dAnimationDatablock(SERPENT_IDLE) {
		imageMap = "enemy_cell_00ImageMap";
		animationFrames = "70 71 69 68";
		animationTime = "0.8";
		animationCycle = "1";
		randomStart = "0";
		startframe = "0";
		pingPong = "0";
		playForward = "1";
	};
	new t2dAnimationDatablock(SERPENT_ATTACK) {
		imageMap = "enemy_cell_00ImageMap";
		animationFrames = "72 73 74 75 75 75";
		animationTime = "0.5";
		animationCycle = "0";
		randomStart = "0";
		startframe = "0";
		pingPong = "0";
		playForward = "1";
	};
	new t2dAnimationDatablock(SERPENT_DEATH) {
		imageMap = "enemy_cell_00ImageMap";
		animationFrames = "76 77 78 79 80";
		animationTime = "0.166666";
		animationCycle = "0";
		randomStart = "0";
		startframe = "0";
		pingPong = "0";
		playForward = "1";
	};
	new t2dAnimationDatablock(MANA_THIEF_MOVE) {
		imageMap = "enemy_cell_00ImageMap";
		animationFrames = "96 97 98 99";
		animationTime = "0.444444";
		animationCycle = "1";
		randomStart = "0";
		startframe = "0";
		pingPong = "0";
		playForward = "1";
	};
	new t2dAnimationDatablock(MANA_THIEF_EAT) {
      imageMap = "enemy_cell_00ImageMap";
      animationFrames = "100 101 102 103";
      animationTime = "0.5";
      animationCycle = "1";
      randomStart = "0";
      startframe = "0";
      pingPong = "0";
      playForward = "1";
   };
	new t2dAnimationDatablock(MANA_THIEF_DRAIN) {
		imageMap = "enemy_cell_00ImageMap";
		animationFrames = "104 105 106 107";
		animationTime = "0.4";
		animationCycle = "1";
		randomStart = "0";
		startframe = "0";
		pingPong = "0";
		playForward = "1";
	};
	new t2dAnimationDatablock(MANA_THIEF_DEATH) {
      imageMap = "enemy_cell_00ImageMap";
      animationFrames = "108 109 110 111";
      animationTime = "0.2";
      animationCycle = "0";
      randomStart = "0";
      startframe = "0";
      pingPong = "0";
      playForward = "1";
   };
	new t2dAnimationDatablock(MANA_THIEF_SPAWN) {
		imageMap = "enemy_cell_00ImageMap";
		animationFrames = "111 110 109 108 96";
		animationTime = "0.166666";
		animationCycle = "0";
		randomStart = "0";
		startframe = "0";
		pingPong = "0";
		playForward = "1";
	};
	new t2dAnimationDatablock(VINE_KING_TAKING_DAMAGE) {
		imageMap = "vineking_cells_000ImageMap";
		animationFrames = "8 9 8 9";
		animationTime = "0.666667";
		animationCycle = "0";
		randomStart = "0";
		startframe = "0";
		pingPong = "0";
		playForward = "1";
	};
	new t2dAnimationDatablock(BLOB_SPAWN) {
		imageMap = "enemy_cell_00ImageMap";
		animationFrames = "3 3 4 5 6 8 9";
		animationTime = "0.875";
		animationCycle = "0";
		randomStart = "0";
		startframe = "0";
		pingPong = "0";
		playForward = "1";
	};
	new t2dAnimationDatablock(BLOB_BARF) {
		imageMap = "enemy_cell_00ImageMap";
		animationFrames = "12 12 13 14 14 15";
		animationTime = "0.428571";
		animationCycle = "0";
		randomStart = "0";
		startframe = "0";
		pingPong = "0";
		playForward = "1";
	};
	new t2dAnimationDatablock(BLOB_DEATH) {
      imageMap = "enemy_cell_00ImageMap";
      animationFrames = "18 19 20 21 22 23 24 24";
      animationTime = "0.2";
      animationCycle = "0";
      randomStart = "0";
      startframe = "0";
      pingPong = "0";
      playForward = "1";
   };
	new t2dAnimationDatablock(BLOB_ATTACK) {
		imageMap = "enemy_cell_00ImageMap";
		animationFrames = "25 26 27 26 27";
		animationTime = "1";
		animationCycle = "0";
		randomStart = "0";
		startframe = "0";
		pingPong = "0";
		playForward = "1";
	};
	new t2dAnimationDatablock(BLOB_MOVE) {
		imageMap = "enemy_cell_00ImageMap";
		animationFrames = "11 5 5 6 7 8 9";
		animationTime = "1";
		animationCycle = "1";
		randomStart = "0";
		startframe = "0";
		pingPong = "0";
		playForward = "1";
	};
	new t2dAnimationDatablock(BLOB_pre_SPAWN) {
		imageMap = "enemy_cell_00ImageMap";
		animationFrames = "0 0 1 0 0 0 2";
		animationTime = "0.875";
		animationCycle = "0";
		randomStart = "0";
		startframe = "0";
		pingPong = "0";
		playForward = "1";
	};
	new t2dAnimationDatablock(Seed_Green_Animation) {
		imageMap = "frame_00ImageMap";
		animationFrames = "36";
		animationTime = "1";
		animationCycle = "0";
		randomStart = "0";
		startframe = "0";
		pingPong = "0";
		playForward = "1";
	};
	new t2dAnimationDatablock(Seed_Red_Animation) {
		imageMap = "frame_00ImageMap";
		animationFrames = "44";
		animationTime = "1";
		animationCycle = "0";
		randomStart = "0";
		startframe = "0";
		pingPong = "0";
		playForward = "1";
	};
	new t2dImageMapDatablock(attack_vineImageMap) {
		imageName = "~/data/images/attack_vine.png";
		imageMode = "CELL";
		frameCount = "-1";
		filterMode = "NONE";
		filterPad = "0";
		preferPerf = "1";
		cellRowOrder = "1";
		cellOffsetX = "0";
		cellOffsetY = "0";
		cellStrideX = "0";
		cellStrideY = "0";
		cellCountX = "-1";
		cellCountY = "-1";
		cellWidth = "32";
		cellHeight = "512";
		preload = "0";
		allowUnload = "0";
		compressPVR = "0";
		optimised = "0";
		force16bit = "0";
	};
	new t2dAnimationDatablock(attack_vineAnimation) {
		imageMap = "attack_vineImageMap";
		animationFrames = "0 0 1 1";
		animationTime = "0.0666667";
		animationCycle = "0";
		randomStart = "0";
		startframe = "0";
		pingPong = "0";
		playForward = "1";
	};
	new t2dAnimationDatablock(Enemy_Spitter_Projectile) {
		imageMap = "frame_00ImageMap";
		animationFrames = "34 35";
		animationTime = "0.4";
		animationCycle = "1";
		randomStart = "0";
		startframe = "0";
		pingPong = "0";
		playForward = "1";
	};
	new t2dImageMapDatablock(frame_hudImageMap) {
		imageName = "~/data/images/frame_hud.png";
		imageMode = "FULL";
		frameCount = "-1";
		filterMode = "NONE";
		filterPad = "0";
		preferPerf = "1";
		cellRowOrder = "1";
		cellOffsetX = "0";
		cellOffsetY = "0";
		cellStrideX = "0";
		cellStrideY = "0";
		cellCountX = "-1";
		cellCountY = "-1";
		cellWidth = "0";
		cellHeight = "0";
		preload = "0";
		allowUnload = "0";
		compressPVR = "0";
		optimised = "0";
		force16bit = "0";
	};
	new t2dImageMapDatablock(water_01_01ImageMap) {
		imageName = "~/data/images/water_01_01.png";
		imageMode = "FULL";
		frameCount = "-1";
		filterMode = "NONE";
		filterPad = "0";
		preferPerf = "1";
		cellRowOrder = "1";
		cellOffsetX = "0";
		cellOffsetY = "0";
		cellStrideX = "0";
		cellStrideY = "0";
		cellCountX = "-1";
		cellCountY = "-1";
		cellWidth = "0";
		cellHeight = "0";
		preload = "0";
		allowUnload = "0";
		compressPVR = "0";
		optimised = "0";
		force16bit = "0";
	};
	new t2dAnimationDatablock(VINEKING_TAKE_DAMAGE) {
		imageMap = "vineking_cells_000ImageMap";
		animationFrames = "8 9 8";
		animationTime = "0.333333";
		animationCycle = "0";
		randomStart = "0";
		startframe = "0";
		pingPong = "0";
		playForward = "1";
	};
	new t2dImageMapDatablock(score_screen_01ImageMap) {
		imageName = "~/data/images/score_screen_01.png";
		imageMode = "FULL";
		frameCount = "-1";
		filterMode = "NONE";
		filterPad = "0";
		preferPerf = "1";
		cellRowOrder = "1";
		cellOffsetX = "0";
		cellOffsetY = "0";
		cellStrideX = "0";
		cellStrideY = "0";
		cellCountX = "-1";
		cellCountY = "-1";
		cellWidth = "0";
		cellHeight = "0";
		preload = "0";
		allowUnload = "0";
		compressPVR = "0";
		optimised = "0";
		force16bit = "0";
	};
	new t2dImageMapDatablock(vine_king_fall_introImageMap) {
		imageName = "~/data/images/vine_king_fall_intro.png";
		imageMode = "CELL";
		frameCount = "-1";
		filterMode = "NONE";
		filterPad = "0";
		preferPerf = "1";
		cellRowOrder = "1";
		cellOffsetX = "0";
		cellOffsetY = "0";
		cellStrideX = "0";
		cellStrideY = "0";
		cellCountX = "-1";
		cellCountY = "-1";
		cellWidth = "64";
		cellHeight = "512";
		preload = "0";
		allowUnload = "0";
		compressPVR = "0";
		optimised = "0";
		force16bit = "0";
	};
	new t2dAnimationDatablock(vine_king_fall_introAnimation) {
		imageMap = "vine_king_fall_introImageMap";
		animationFrames = "0 1 2 3 4 5 6 7";
		animationTime = "0.8";
		animationCycle = "0";
		randomStart = "0";
		startframe = "0";
		pingPong = "0";
		playForward = "1";
	};
	new t2dImageMapDatablock(vine_king_finishing_moveImageMap) {
		imageName = "~/data/images/vine_king_finishing_move.png";
		imageMode = "CELL";
		frameCount = "-1";
		filterMode = "NONE";
		filterPad = "0";
		preferPerf = "1";
		cellRowOrder = "1";
		cellOffsetX = "0";
		cellOffsetY = "0";
		cellStrideX = "0";
		cellStrideY = "0";
		cellCountX = "-1";
		cellCountY = "-1";
		cellWidth = "64";
		cellHeight = "256";
		preload = "0";
		allowUnload = "0";
		compressPVR = "0";
		optimised = "0";
		force16bit = "0";
	};
	new t2dAnimationDatablock(vine_king_finishing_moveAnimation) {
		imageMap = "vine_king_finishing_moveImageMap";
		animationFrames = "0 0 1 1 2 2 3 3 4 5 6 7 8 9 10 10 11 11 12 12 12 13 13 13 14 14 14 15 15 15";
		animationTime = "2";
		animationCycle = "0";
		randomStart = "0";
		startframe = "0";
		pingPong = "0";
		playForward = "1";
	};
	new t2dImageMapDatablock(vine_king_fall_fxImageMap) {
		imageName = "~/data/images/vine_king_fall_fx.png";
		imageMode = "CELL";
		frameCount = "-1";
		filterMode = "NONE";
		filterPad = "0";
		preferPerf = "1";
		cellRowOrder = "1";
		cellOffsetX = "0";
		cellOffsetY = "0";
		cellStrideX = "0";
		cellStrideY = "0";
		cellCountX = "-1";
		cellCountY = "-1";
		cellWidth = "128";
		cellHeight = "128";
		preload = "0";
		allowUnload = "0";
		compressPVR = "0";
		optimised = "0";
		force16bit = "0";
	};
	new t2dAnimationDatablock(vine_king_fall_fxAnimation) {
		imageMap = "vine_king_fall_fxImageMap";
		animationFrames = "0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15";
		animationTime = "1.33333";
		animationCycle = "1";
		randomStart = "0";
		startframe = "0";
		pingPong = "0";
		playForward = "1";
	};
	new t2dImageMapDatablock(win_loseImageMap) {
		imageName = "~/data/images/win_lose.png";
		imageMode = "CELL";
		frameCount = "-1";
		filterMode = "NONE";
		filterPad = "0";
		preferPerf = "1";
		cellRowOrder = "1";
		cellOffsetX = "0";
		cellOffsetY = "0";
		cellStrideX = "0";
		cellStrideY = "0";
		cellCountX = "-1";
		cellCountY = "-1";
		cellWidth = "256";
		cellHeight = "64";
		preload = "0";
		allowUnload = "0";
		compressPVR = "0";
		optimised = "0";
		force16bit = "0";
	};
	new t2dImageMapDatablock(buttons_newImageMap) {
		imageName = "~/data/images/buttons_new.png";
		imageMode = "CELL";
		frameCount = "-1";
		filterMode = "NONE";
		filterPad = "0";
		preferPerf = "1";
		cellRowOrder = "1";
		cellOffsetX = "0";
		cellOffsetY = "0";
		cellStrideX = "0";
		cellStrideY = "0";
		cellCountX = "-1";
		cellCountY = "-1";
		cellWidth = "64";
		cellHeight = "64";
		preload = "0";
		allowUnload = "0";
		compressPVR = "0";
		optimised = "0";
		force16bit = "0";
	};
	new t2dImageMapDatablock(pause_screenImageMap) {
		imageName = "~/data/images/pause_screen.png";
		imageMode = "FULL";
		frameCount = "-1";
		filterMode = "NONE";
		filterPad = "0";
		preferPerf = "1";
		cellRowOrder = "1";
		cellOffsetX = "0";
		cellOffsetY = "0";
		cellStrideX = "0";
		cellStrideY = "0";
		cellCountX = "-1";
		cellCountY = "-1";
		cellWidth = "0";
		cellHeight = "0";
		preload = "0";
		allowUnload = "0";
		compressPVR = "0";
		optimised = "0";
		force16bit = "0";
	};
	new t2dImageMapDatablock(map_button_newImageMap) {
		imageName = "~/data/images/map_button_new.png";
		imageMode = "FULL";
		frameCount = "-1";
		filterMode = "NONE";
		filterPad = "0";
		preferPerf = "1";
		cellRowOrder = "1";
		cellOffsetX = "0";
		cellOffsetY = "0";
		cellStrideX = "0";
		cellStrideY = "0";
		cellCountX = "-1";
		cellCountY = "-1";
		cellWidth = "0";
		cellHeight = "0";
		preload = "0";
		allowUnload = "0";
		compressPVR = "0";
		optimised = "0";
		force16bit = "0";
	};
	new t2dImageMapDatablock(quit_screenImageMap) {
		imageName = "~/data/images/quit_screen.png";
		imageMode = "FULL";
		frameCount = "-1";
		filterMode = "NONE";
		filterPad = "0";
		preferPerf = "1";
		cellRowOrder = "1";
		cellOffsetX = "0";
		cellOffsetY = "0";
		cellStrideX = "0";
		cellStrideY = "0";
		cellCountX = "-1";
		cellCountY = "-1";
		cellWidth = "0";
		cellHeight = "0";
		preload = "0";
		allowUnload = "0";
		compressPVR = "0";
		optimised = "0";
		force16bit = "0";
	};
	new t2dImageMapDatablock(yes_no_buttonsImageMap) {
		imageName = "~/data/images/yes_no_buttons.png";
		imageMode = "CELL";
		frameCount = "-1";
		filterMode = "NONE";
		filterPad = "0";
		preferPerf = "1";
		cellRowOrder = "1";
		cellOffsetX = "0";
		cellOffsetY = "0";
		cellStrideX = "0";
		cellStrideY = "0";
		cellCountX = "-1";
		cellCountY = "-1";
		cellWidth = "128";
		cellHeight = "64";
		preload = "0";
		allowUnload = "0";
		compressPVR = "0";
		optimised = "0";
		force16bit = "0";
	};
	new t2dImageMapDatablock(score_screen_starsImageMap) {
		imageName = "~/data/images/score_screen_stars.png";
		imageMode = "CELL";
		frameCount = "-1";
		filterMode = "NONE";
		filterPad = "0";
		preferPerf = "1";
		cellRowOrder = "1";
		cellOffsetX = "0";
		cellOffsetY = "0";
		cellStrideX = "0";
		cellStrideY = "0";
		cellCountX = "-1";
		cellCountY = "-1";
		cellWidth = "128";
		cellHeight = "64";
		preload = "0";
		allowUnload = "0";
		compressPVR = "0";
		optimised = "0";
		force16bit = "0";
	};
	new t2dImageMapDatablock(_4x4_White_SquareImageMap) {
		imageName = "~/data/images/4x4_White_Square.png";
		imageMode = "FULL";
		frameCount = "-1";
		filterMode = "NONE";
		filterPad = "0";
		preferPerf = "1";
		cellRowOrder = "1";
		cellOffsetX = "0";
		cellOffsetY = "0";
		cellStrideX = "0";
		cellStrideY = "0";
		cellCountX = "-1";
		cellCountY = "-1";
		cellWidth = "0";
		cellHeight = "0";
		preload = "0";
		allowUnload = "0";
		compressPVR = "0";
		optimised = "0";
		force16bit = "0";
	};
	new t2dImageMapDatablock(Achievement_BannerImageMap) {
		imageName = "~/data/images/Achievement_Banner.png";
		imageMode = "FULL";
		frameCount = "-1";
		filterMode = "NONE";
		filterPad = "0";
		preferPerf = "1";
		cellRowOrder = "1";
		cellOffsetX = "0";
		cellOffsetY = "0";
		cellStrideX = "0";
		cellStrideY = "0";
		cellCountX = "-1";
		cellCountY = "-1";
		cellWidth = "0";
		cellHeight = "0";
		preload = "0";
		allowUnload = "0";
		compressPVR = "0";
		optimised = "0";
		force16bit = "0";
	};
	new t2dImageMapDatablock(corner_buttonImageMap) {
		imageName = "~/data/images/corner_button.png";
		imageMode = "FULL";
		frameCount = "-1";
		filterMode = "NONE";
		filterPad = "0";
		preferPerf = "1";
		cellRowOrder = "1";
		cellOffsetX = "0";
		cellOffsetY = "0";
		cellStrideX = "0";
		cellStrideY = "0";
		cellCountX = "-1";
		cellCountY = "-1";
		cellWidth = "0";
		cellHeight = "0";
		preload = "0";
		allowUnload = "0";
		compressPVR = "0";
		optimised = "0";
		force16bit = "0";
	};
	new t2dImageMapDatablock(hit_fxImageMap) {
		imageName = "~/data/images/hit_fx.png";
		imageMode = "CELL";
		frameCount = "-1";
		filterMode = "NONE";
		filterPad = "0";
		preferPerf = "1";
		cellRowOrder = "1";
		cellOffsetX = "0";
		cellOffsetY = "0";
		cellStrideX = "0";
		cellStrideY = "0";
		cellCountX = "-1";
		cellCountY = "-1";
		cellWidth = "128";
		cellHeight = "128";
		preload = "0";
		allowUnload = "0";
		compressPVR = "0";
		optimised = "0";
		force16bit = "0";
	};
	new t2dAnimationDatablock(hit_fxAnimation) {
		imageMap = "hit_fxImageMap";
		animationFrames = "0 1 2 3";
		animationTime = "0.25";
		animationCycle = "0";
		randomStart = "0";
		startframe = "0";
		pingPong = "0";
		playForward = "1";
	};
	new t2dImageMapDatablock(vortex_fx_cellImageMap) {
		imageName = "~/data/images/vortex_fx_cell.png";
		imageMode = "CELL";
		frameCount = "-1";
		filterMode = "NONE";
		filterPad = "0";
		preferPerf = "1";
		cellRowOrder = "1";
		cellOffsetX = "0";
		cellOffsetY = "0";
		cellStrideX = "0";
		cellStrideY = "0";
		cellCountX = "-1";
		cellCountY = "-1";
		cellWidth = "64";
		cellHeight = "64";
		preload = "0";
		allowUnload = "0";
		compressPVR = "0";
		optimised = "0";
		force16bit = "0";
	};
	new t2dAnimationDatablock(vortex_fx_cellAnimation) {
		imageMap = "vortex_fx_cellImageMap";
		animationFrames = "0 1 2 3 4 5 6 7";
		animationTime = "1";
		animationCycle = "1";
		randomStart = "0";
		startframe = "0";
		pingPong = "0";
		playForward = "1";
	};
	new t2dImageMapDatablock(seed_buttonImageMap) {
		imageName = "~/data/images/seed_button.png";
		imageMode = "FULL";
		frameCount = "-1";
		filterMode = "NONE";
		filterPad = "0";
		preferPerf = "1";
		cellRowOrder = "1";
		cellOffsetX = "0";
		cellOffsetY = "0";
		cellStrideX = "0";
		cellStrideY = "0";
		cellCountX = "-1";
		cellCountY = "-1";
		cellWidth = "0";
		cellHeight = "0";
		preload = "0";
		allowUnload = "0";
		compressPVR = "0";
		optimised = "0";
		force16bit = "0";
	};
	new t2dImageMapDatablock(smoke_hit_fxImageMap) {
		imageName = "~/data/images/smoke_hit_fx.png";
		imageMode = "CELL";
		frameCount = "-1";
		filterMode = "NONE";
		filterPad = "0";
		preferPerf = "1";
		cellRowOrder = "1";
		cellOffsetX = "0";
		cellOffsetY = "0";
		cellStrideX = "0";
		cellStrideY = "0";
		cellCountX = "-1";
		cellCountY = "-1";
		cellWidth = "128";
		cellHeight = "128";
		preload = "0";
		allowUnload = "0";
		compressPVR = "0";
		optimised = "0";
		force16bit = "0";
	};
	new t2dAnimationDatablock(smoke_hit_fxAnimation) {
		imageMap = "smoke_hit_fxImageMap";
		animationFrames = "0 1 2 3";
		animationTime = "0.25";
		animationCycle = "0";
		randomStart = "0";
		startframe = "0";
		pingPong = "0";
		playForward = "1";
	};
	new t2dAnimationDatablock(DANGER_BRICK_DAMAGE) {
		imageMap = "enemy_cell_00ImageMap";
		animationFrames = "0 1 1 2 2 1 1 2 2 1 1 2 2 1 1 2 2 1 1 2 2 0";
		animationTime = "1";
		animationCycle = "0";
		randomStart = "0";
		startframe = "0";
		pingPong = "0";
		playForward = "1";
	};
	//----------------------------------------------------------------------------------------------
	// THE FOLLOWING DATABLOCKS ARE FOR THE TUTORIAL LEVEL ONLY
	new t2dImageMapDatablock(intro_line_drawImageMap) {
		imageName = "~/data/images/intro_line_draw.png";
		imageMode = "FULL";
		frameCount = "-1";
		filterMode = "NONE";
		filterPad = "0";
		preferPerf = "1";
		cellRowOrder = "1";
		cellOffsetX = "0";
		cellOffsetY = "0";
		cellStrideX = "0";
		cellStrideY = "0";
		cellCountX = "-1";
		cellCountY = "-1";
		cellWidth = "0";
		cellHeight = "0";
		preload = "0";
		allowUnload = "0";
		compressPVR = "0";
		optimised = "0";
		force16bit = "0";
	};
	new t2dImageMapDatablock(intro_seed_suckImageMap) {
		imageName = "~/data/images/intro_seed_suck.png";
		imageMode = "FULL";
		frameCount = "-1";
		filterMode = "NONE";
		filterPad = "0";
		preferPerf = "1";
		cellRowOrder = "1";
		cellOffsetX = "0";
		cellOffsetY = "0";
		cellStrideX = "0";
		cellStrideY = "0";
		cellCountX = "-1";
		cellCountY = "-1";
		cellWidth = "0";
		cellHeight = "0";
		preload = "0";
		allowUnload = "0";
		compressPVR = "0";
		optimised = "0";
		force16bit = "0";
	};
	new t2dImageMapDatablock(intro_blavaImageMap) {
		imageName = "~/data/images/intro_blava.png";
		imageMode = "FULL";
		frameCount = "-1";
		filterMode = "NONE";
		filterPad = "0";
		preferPerf = "1";
		cellRowOrder = "1";
		cellOffsetX = "0";
		cellOffsetY = "0";
		cellStrideX = "0";
		cellStrideY = "0";
		cellCountX = "-1";
		cellCountY = "-1";
		cellWidth = "0";
		cellHeight = "0";
		preload = "0";
		allowUnload = "0";
		compressPVR = "0";
		optimised = "0";
		force16bit = "0";
	};
	new t2dImageMapDatablock(intro_vine_attackImageMap) {
		imageName = "~/data/images/intro_vine_attack.png";
		imageMode = "FULL";
		frameCount = "-1";
		filterMode = "NONE";
		filterPad = "0";
		preferPerf = "1";
		cellRowOrder = "1";
		cellOffsetX = "0";
		cellOffsetY = "0";
		cellStrideX = "0";
		cellStrideY = "0";
		cellCountX = "-1";
		cellCountY = "-1";
		cellWidth = "0";
		cellHeight = "0";
		preload = "0";
		allowUnload = "0";
		compressPVR = "0";
		optimised = "0";
		force16bit = "0";
	};
	new t2dImageMapDatablock(intro_damageImageMap) {
		imageName = "~/data/images/intro_damage.png";
		imageMode = "FULL";
		frameCount = "-1";
		filterMode = "NONE";
		filterPad = "0";
		preferPerf = "1";
		cellRowOrder = "1";
		cellOffsetX = "0";
		cellOffsetY = "0";
		cellStrideX = "0";
		cellStrideY = "0";
		cellCountX = "-1";
		cellCountY = "-1";
		cellWidth = "0";
		cellHeight = "0";
		preload = "0";
		allowUnload = "0";
		compressPVR = "0";
		optimised = "0";
		force16bit = "0";
	};
	new t2dImageMapDatablock(FACEBOOK_ICONImageMap) {
		imageName = "~/data/images/FACEBOOK_ICON.PNG";
		imageMode = "FULL";
		frameCount = "-1";
		filterMode = "NONE";
		filterPad = "0";
		preferPerf = "1";
		cellRowOrder = "1";
		cellOffsetX = "0";
		cellOffsetY = "0";
		cellStrideX = "0";
		cellStrideY = "0";
		cellCountX = "-1";
		cellCountY = "-1";
		cellWidth = "0";
		cellHeight = "0";
		preload = "0";
		allowUnload = "0";
		compressPVR = "0";
		optimised = "0";
		force16bit = "0";
	};
   new t2dImageMapDatablock(TT01ImageMap) {
      imageName = "~/data/images/TT01.png";
      imageMode = "FULL";
      frameCount = "-1";
      filterMode = "NONE";
      filterPad = "0";
      preferPerf = "1";
      cellRowOrder = "1";
      cellOffsetX = "0";
      cellOffsetY = "0";
      cellStrideX = "0";
      cellStrideY = "0";
      cellCountX = "-1";
      cellCountY = "-1";
      cellWidth = "0";
      cellHeight = "0";
      preload = "0";
      allowUnload = "0";
      compressPVR = "0";
      optimised = "0";
      force16bit = "0";
   };
   new t2dImageMapDatablock(TT02ImageMap) {
      imageName = "~/data/images/TT02.png";
      imageMode = "FULL";
      frameCount = "-1";
      filterMode = "NONE";
      filterPad = "0";
      preferPerf = "1";
      cellRowOrder = "1";
      cellOffsetX = "0";
      cellOffsetY = "0";
      cellStrideX = "0";
      cellStrideY = "0";
      cellCountX = "-1";
      cellCountY = "-1";
      cellWidth = "0";
      cellHeight = "0";
      preload = "0";
      allowUnload = "0";
      compressPVR = "0";
      optimised = "0";
      force16bit = "0";
   };
   new t2dImageMapDatablock(TT03ImageMap) {
      imageName = "~/data/images/TT03.PNG";
      imageMode = "FULL";
      frameCount = "-1";
      filterMode = "NONE";
      filterPad = "0";
      preferPerf = "1";
      cellRowOrder = "1";
      cellOffsetX = "0";
      cellOffsetY = "0";
      cellStrideX = "0";
      cellStrideY = "0";
      cellCountX = "-1";
      cellCountY = "-1";
      cellWidth = "0";
      cellHeight = "0";
      preload = "0";
      allowUnload = "0";
      compressPVR = "0";
      optimised = "0";
      force16bit = "0";
   };
   new t2dImageMapDatablock(TT04ImageMap) {
      imageName = "~/data/images/TT04.PNG";
      imageMode = "FULL";
      frameCount = "-1";
      filterMode = "NONE";
      filterPad = "0";
      preferPerf = "1";
      cellRowOrder = "1";
      cellOffsetX = "0";
      cellOffsetY = "0";
      cellStrideX = "0";
      cellStrideY = "0";
      cellCountX = "-1";
      cellCountY = "-1";
      cellWidth = "0";
      cellHeight = "0";
      preload = "0";
      allowUnload = "0";
      compressPVR = "0";
      optimised = "0";
      force16bit = "0";
   };
   new t2dImageMapDatablock(TT05ImageMap) {
      imageName = "~/data/images/TT05.PNG";
      imageMode = "FULL";
      frameCount = "-1";
      filterMode = "NONE";
      filterPad = "0";
      preferPerf = "1";
      cellRowOrder = "1";
      cellOffsetX = "0";
      cellOffsetY = "0";
      cellStrideX = "0";
      cellStrideY = "0";
      cellCountX = "-1";
      cellCountY = "-1";
      cellWidth = "0";
      cellHeight = "0";
      preload = "0";
      allowUnload = "0";
      compressPVR = "0";
      optimised = "0";
      force16bit = "0";
   };
   new t2dImageMapDatablock(TT06ImageMap) {
      imageName = "~/data/images/TT06.PNG";
      imageMode = "FULL";
      frameCount = "-1";
      filterMode = "NONE";
      filterPad = "0";
      preferPerf = "1";
      cellRowOrder = "1";
      cellOffsetX = "0";
      cellOffsetY = "0";
      cellStrideX = "0";
      cellStrideY = "0";
      cellCountX = "-1";
      cellCountY = "-1";
      cellWidth = "0";
      cellHeight = "0";
      preload = "0";
      allowUnload = "0";
      compressPVR = "0";
      optimised = "0";
      force16bit = "0";
   };
   new t2dImageMapDatablock(TT07ImageMap) {
      imageName = "~/data/images/TT07.PNG";
      imageMode = "FULL";
      frameCount = "-1";
      filterMode = "NONE";
      filterPad = "0";
      preferPerf = "1";
      cellRowOrder = "1";
      cellOffsetX = "0";
      cellOffsetY = "0";
      cellStrideX = "0";
      cellStrideY = "0";
      cellCountX = "-1";
      cellCountY = "-1";
      cellWidth = "0";
      cellHeight = "0";
      preload = "0";
      allowUnload = "0";
      compressPVR = "0";
      optimised = "0";
      force16bit = "0";
   };
   new t2dImageMapDatablock(TT08ImageMap) {
      imageName = "~/data/images/TT08.PNG";
      imageMode = "FULL";
      frameCount = "-1";
      filterMode = "NONE";
      filterPad = "0";
      preferPerf = "1";
      cellRowOrder = "1";
      cellOffsetX = "0";
      cellOffsetY = "0";
      cellStrideX = "0";
      cellStrideY = "0";
      cellCountX = "-1";
      cellCountY = "-1";
      cellWidth = "0";
      cellHeight = "0";
      preload = "0";
      allowUnload = "0";
      compressPVR = "0";
      optimised = "0";
      force16bit = "0";
   };
   new t2dImageMapDatablock(TT09ImageMap) {
      imageName = "~/data/images/TT09.PNG";
      imageMode = "FULL";
      frameCount = "-1";
      filterMode = "NONE";
      filterPad = "0";
      preferPerf = "1";
      cellRowOrder = "1";
      cellOffsetX = "0";
      cellOffsetY = "0";
      cellStrideX = "0";
      cellStrideY = "0";
      cellCountX = "-1";
      cellCountY = "-1";
      cellWidth = "0";
      cellHeight = "0";
      preload = "0";
      allowUnload = "0";
      compressPVR = "0";
      optimised = "0";
      force16bit = "0";
   };
   new t2dImageMapDatablock(TT10ImageMap) {
      imageName = "~/data/images/TT10.PNG";
      imageMode = "FULL";
      frameCount = "-1";
      filterMode = "NONE";
      filterPad = "0";
      preferPerf = "1";
      cellRowOrder = "1";
      cellOffsetX = "0";
      cellOffsetY = "0";
      cellStrideX = "0";
      cellStrideY = "0";
      cellCountX = "-1";
      cellCountY = "-1";
      cellWidth = "0";
      cellHeight = "0";
      preload = "0";
      allowUnload = "0";
      compressPVR = "0";
      optimised = "0";
      force16bit = "0";
   };
   new t2dImageMapDatablock(TT11ImageMap) {
      imageName = "~/data/images/TT11.PNG";
      imageMode = "FULL";
      frameCount = "-1";
      filterMode = "NONE";
      filterPad = "0";
      preferPerf = "1";
      cellRowOrder = "1";
      cellOffsetX = "0";
      cellOffsetY = "0";
      cellStrideX = "0";
      cellStrideY = "0";
      cellCountX = "-1";
      cellCountY = "-1";
      cellWidth = "0";
      cellHeight = "0";
      preload = "0";
      allowUnload = "0";
      compressPVR = "0";
      optimised = "0";
      force16bit = "0";
   };
   new t2dImageMapDatablock(finger_off_onImageMap) {
      imageName = "~/data/images/finger_off_on.png";
      imageMode = "CELL";
      frameCount = "-1";
      filterMode = "NONE";
      filterPad = "1";
      preferPerf = "1";
      cellRowOrder = "1";
      cellOffsetX = "0";
      cellOffsetY = "0";
      cellStrideX = "0";
      cellStrideY = "0";
      cellCountX = "-1";
      cellCountY = "-1";
      cellWidth = "256";
      cellHeight = "256";
      preload = "0";
      allowUnload = "0";
      compressPVR = "0";
      optimised = "0";
      force16bit = "0";
   };
   new t2dAnimationDatablock(finger_off_onAnimation) {
      imageMap = "finger_off_onImageMap";
      animationFrames = "0 1";
      animationTime = "1";
      animationCycle = "1";
      randomStart = "0";
      startframe = "0";
      pingPong = "0";
      playForward = "1";
   };
	//================================================================================================
	// Audio Descriptions
	//================================================================================================
	
	new AudioDescription( AudioLooping )  
	{  
		volume = 1.0;
		isLooping = true;
		is3D = false;
		type = 1;
		isStreaming = false;
	}; 
	
	new AudioDescription( AudioBGMNonLooping )  
	{  
		volume = 1.0;
		isLooping = false;
		is3D = false;
		type = 2;
		isStreaming = false;
	};
	
	new AudioDescription( AudioNonLooping )
	{
		volume = 1.0;
		isLooping = false;
		is3D = false;
		type = 3;
		isStreaming = false;
	};
	
	//-----------------------------------------------------------------------------------------------
	// BGM
	//-----------------------------------------------------------------------------------------------
	
	//----------------------------------------
	// BGM Lose
	new AudioProfile( BGMLose )
	{
		filename = "~/data/audio/P02_BGM_Lose.wav";
		description = "AudioBGMNonLooping";
		preload = false;
	};
	
	//----------------------------------------
	// BGM Win
	new AudioProfile( BGMWin )
	{
		filename = "~/data/audio/P02_BGM_Win.wav";
		description = "AudioBGMNonLooping";
		preload = false;
	};
	
	//----------------------------------------
	// BGM In Game Music
	new AudioProfile( BGMGameMusic )
	{
		filename = "~/data/audio/P02_BGM_Level_Select.wav";
		description = "AudioLooping";
		preload = false;
	};
	
	//-----------------------------------------------------------------------------------------------
	// Sound Effects
	//-----------------------------------------------------------------------------------------------
	
	//----------------------------------------
	// Lava Tile Shatter
	new AudioProfile( LavaTileShatter )
	{
		filename = "~/data/audio/P02_SFX_Rock_Smash.wav";
		description = "AudioNonLooping";
		preload = false;
	};
	
	//----------------------------------------
	// Crystal Shatter
	//new AudioProfile( CrystalShatter )
	//{
	//	filename = "~/data/audio/P02_Shatter.wav";
	//	description = "AudioNonLooping";
	//	preload = false;
	//};
	
	//----------------------------------------
	// Player Chanting
	new AudioProfile( PlayerChanting )
	{
		filename = "~/data/audio/P02_SFX_Player_Chant.wav";
		description = "AudioNonLooping";
		preload = false;
	};
	
	//----------------------------------------
	// Player Take Damage
	new AudioProfile( PlayerTakeDamage )
	{
		filename = "~/data/audio/P02_SFX_Player_Hit.wav";
		description = "AudioNonLooping";
		preload = false;
	};
	
	//----------------------------------------
	// Player Eat
	new AudioProfile( PlayerEat )
	{
		filename = "~/data/audio/P02_SFX_Player_Eat.wav";
		description = "AudioNonLooping";
		preload = false;
	};
	
	//----------------------------------------
	// Player Death
	new AudioProfile( PlayerDeath )
	{
		filename = "~/data/audio/P02_SFX_Player_Death.wav";
		description = "AudioNonLooping";
		preload = false;
	};
	
	//----------------------------------------
	// Out of Mana
	new AudioProfile( OutOfMana )
	{
		filename = "~/data/audio/P02_Mana_Out.wav";
		description = "AudioNonLooping";
		preload = false;
	};
	
	//----------------------------------------
	// Vortex Sound
	new AudioProfile( VortexSound )
	{
		filename = "~/data/audio/P02_SFX_Vortex.wav";
		description = "AudioNonLooping";
		preload = false;
	};
	
	//----------------------------------------
	// Attack Tap
	new AudioProfile( AttackTap )
	{
		filename = "~/data/audio/P02_SFX_Attack_Tap.wav";
		description = "AudioNonLooping";
		preload = false;
	};
	
	//----------------------------------------
	// Enemy Spawn Sound
	new AudioProfile( EnemySpawn )
	{
		filename = "~/data/audio/P02_SFX_Enemy_Spawn.wav";
		description = "AudioNonLooping";
		preload = false;
	};
	
	//----------------------------------------
	// Enemy A Puke (Blob Brick Attack)
	new AudioProfile( BlobPuke )
	{
		filename = "~/data/audio/P02_SFX_PUKE_Blob.wav";
		description = "AudioNonLooping";
		preload = false;
	};
	
	//----------------------------------------
	// Enemy A Death (Blob Death)
	new AudioProfile( BlobDeath )
	{
		filename = "~/data/audio/P02_SFX_Death_A.wav";
		description = "AudioNonLooping";
		preload = false;
	};
	
	//----------------------------------------
	// Enemy B Attack (Serpent Attack)
	//new AudioProfile( SerpentAttack )
	//{
	//	filename = "~/data/audio/P02_SFX_ATK_Spit.wav";
	//	description = "AudioNonLooping";
	//	preload = false;
	//};
	
	//----------------------------------------
	// Enemy B Death (Serpent Death)
	//new AudioProfile( SerpentDeath )
	//{
	//	filename = "~/data/audio/P02_SFX_Death_B.wav";
	//	description = "AudioNonLooping";
	//	preload = false;
	//};
	
	//----------------------------------------
	// Enemy D Spawn (Smoke Spawn)
	//new AudioProfile( SmokeSpawn )
	//{
	//	filename = "~/data/audio/P02_SFX_SPAWN_Smoke.wav";
	//	description = "AudioNonLooping";
	//	preload = false;
	//};
	
	//----------------------------------------
	// Enemy D Death (Smoke Monster Death)
	//new AudioProfile( SmokeMonsterDeath )
	//{
	//	filename = "~/data/audio/P02_SFX_Death_D.wav";
	//	description = "AudioNonLooping";
	//	preload = false;
	//};
	//----------------------------------------
	// Button Clicks
	new AudioProfile( ButtonClickForward )
	{
		filename = "~/data/audio/P02_SFX_Click_F.wav";
		description = "AudioNonLooping";
		preload = false;
	};
	new AudioProfile( ButtonClickBack )
	{
		filename = "~/data/audio/P02_SFX_Click_B.wav";
		description = "AudioNonLooping";
		preload = false;
	};
	
	//----------------------------------------
	// Intro / Outro Sounds
	new AudioProfile( IntroFallSound )
	{
		filename = "~/data/audio/P02_SFX_INTRO_FALL.wav";
		description = "AudioNonLooping";
		preload = false;
	};
	
	new AudioProfile( ThudSound )
	{
		filename = "~/data/audio/P02_SFX_THUD.wav";
		description = "AudioNonLooping";
		preload = false;
	};
	
	new AudioProfile( WinGruntSound )
	{
		filename = "~/data/audio/P02_SFX_WIN_GRUNT.wav";
		description = "AudioNonLooping";
		preload = false;
	};
	
	new AudioProfile( FinalGrowSound )
	{
		filename = "~/data/audio/P02_SFX_FINAL_GROW.wav";
		description = "AudioBGMNonLooping";
		preload = false;
	};
	
	new AudioProfile( TutorialStinger )
	{
		filename = "~/data/audio/P02_BGM_Intro_Stinger.wav";
		description = "AudioBGMNonLooping";
		preload = false;
	};
};