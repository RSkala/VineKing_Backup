$levelDatablocks = new SimSet() {
   canSaveDynamicFields = "1";
      setType = "Datablocks";

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
	
	//----------------------------------------
	new AudioProfile( BGM_MapSelectScreen )
	{
		filename = "~/data/audio/P02_BGM_Level_Select.wav";
		description = "AudioLooping";
		preload = false;
	};
	
	new AudioProfile( ButtonClickForward )
	{
		filename = "~/data/audio/P02_SFX_Click_F.wav";
		description = "AudioNonLooping";
		preload = false;
	};
};
