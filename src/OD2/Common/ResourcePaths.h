#pragma once

#include <string>

namespace OD2::Common::ResourcePaths {

namespace Language {
const std::string LocalLanguage = "/data/local/use";
const std::string LanguageFontToken = "{LANG_FONT}";
const std::string LanguageTableToken = "{LANG}";
} // namespace Language

namespace Screens {
const std::string LoadingScreen = "/data/global/ui/Loading/loadingscreen.dc6";
}

namespace MainMenu {
const std::string TrademarkScreen = "/data/global/ui/FrontEnd/trademarkscreenEXP.dc6";
const std::string GameSelectScreen = "/data/global/ui/FrontEnd/gameselectscreenEXP.dc6";
const std::string TCPIPBackground = "/data/global/ui/FrontEnd/TCPIPscreen.dc6";
const std::string Diablo2LogoFireLeft = "/data/global/ui/FrontEnd/D2logoFireLeft.DC6";
const std::string Diablo2LogoFireRight = "/data/global/ui/FrontEnd/D2logoFireRight.DC6";
const std::string Diablo2LogoBlackLeft = "/data/global/ui/FrontEnd/D2logoBlackLeft.DC6";
const std::string Diablo2LogoBlackRight = "/data/global/ui/FrontEnd/D2logoBlackRight.DC6";
} // namespace MainMenu

namespace Credits {
const std::string CreditsBackground = "/data/global/ui/CharSelect/creditsbckgexpand.dc6";
const std::string CreditsText = "/data/local/ui/" + Language::LanguageTableToken + "/ExpansionCredits.txt";
} // namespace Credits

namespace Cinematics {
const std::string Background = "/data/global/ui/FrontEnd/CinematicsSelectionEXP.dc6";
}

namespace Videos {
const std::string BlizardStartup1 = "/data/local/video/New_Bliz640x480.bik";
const std::string BlizardStartup2 = "/data/local/video/BlizNorth640x480.bik";
const std::string Act1Intro = "/data/local/video/" + Language::LanguageTableToken + "/d2intro640x292.bik";
const std::string Act2Intro = "/data/local/video/" + Language::LanguageTableToken + "/act02start640x292.bik";
const std::string Act3Intro = "/data/local/video/" + Language::LanguageTableToken + "/act03start640x292.bik";
const std::string Act4Intro = "/data/local/video/" + Language::LanguageTableToken + "/act04start640x292.bik";
const std::string Act4Outro = "/data/local/video/" + Language::LanguageTableToken + "/act04end640x292.bik";
const std::string Act5Intro = "/data/local/video/" + Language::LanguageTableToken + "/d2x_intro_640x292.bik";
const std::string Act5Outro = "/data/local/video/" + Language::LanguageTableToken + "/d2x_out_640x292.bik";
} // namespace Videos

namespace CharacterSelectScreen {
const std::string Background = "/data/global/ui/FrontEnd/charactercreationscreenEXP.dc6";
const std::string Campfire = "/data/global/ui/FrontEnd/fire.DC6";

const std::string BarbarianUnselected = "/data/global/ui/FrontEnd/barbarian/banu1.DC6";
const std::string BarbarianUnselectedH = "/data/global/ui/FrontEnd/barbarian/banu2.DC6";
const std::string BarbarianSelected = "/data/global/ui/FrontEnd/barbarian/banu3.DC6";
const std::string BarbarianForwardWalk = "/data/global/ui/FrontEnd/barbarian/bafw.DC6";
const std::string BarbarianForwardWalkOverlay = "/data/global/ui/FrontEnd/barbarian/BAFWs.DC6";
const std::string BarbarianBackWalk = "/data/global/ui/FrontEnd/barbarian/babw.DC6";

const std::string SorceressUnselected = "/data/global/ui/FrontEnd/sorceress/SONU1.DC6";
const std::string SorceressUnselectedH = "/data/global/ui/FrontEnd/sorceress/SONU2.DC6";
const std::string SorceressSelected = "/data/global/ui/FrontEnd/sorceress/SONU3.DC6";
const std::string SorceressSelectedOverlay = "/data/global/ui/FrontEnd/sorceress/SONU3s.DC6";
const std::string SorceressForwardWalk = "/data/global/ui/FrontEnd/sorceress/SOFW.DC6";
const std::string SorceressForwardWalkOverlay = "/data/global/ui/FrontEnd/sorceress/SOFWs.DC6";
const std::string SorceressBackWalk = "/data/global/ui/FrontEnd/sorceress/SOBW.DC6";
const std::string SorceressBackWalkOverlay = "/data/global/ui/FrontEnd/sorceress/SOBWs.DC6";

const std::string NecromancerUnselected = "/data/global/ui/FrontEnd/necromancer/NENU1.DC6";
const std::string NecromancerUnselectedH = "/data/global/ui/FrontEnd/necromancer/NENU2.DC6";
const std::string NecromancerSelected = "/data/global/ui/FrontEnd/necromancer/NENU3.DC6";
const std::string NecromancerSelectedOverlay = "/data/global/ui/FrontEnd/necromancer/NENU3s.DC6";
const std::string NecromancerForwardWalk = "/data/global/ui/FrontEnd/necromancer/NEFW.DC6";
const std::string NecromancerForwardWalkOverlay = "/data/global/ui/FrontEnd/necromancer/NEFWs.DC6";
const std::string NecromancerBackWalk = "/data/global/ui/FrontEnd/necromancer/NEBW.DC6";
const std::string NecromancerBackWalkOverlay = "/data/global/ui/FrontEnd/necromancer/NEBWs.DC6";

const std::string PaladinUnselected = "/data/global/ui/FrontEnd/paladin/PANU1.DC6";
const std::string PaladinUnselectedH = "/data/global/ui/FrontEnd/paladin/PANU2.DC6";
const std::string PaladinSelected = "/data/global/ui/FrontEnd/paladin/PANU3.DC6";
const std::string PaladinForwardWalk = "/data/global/ui/FrontEnd/paladin/PAFW.DC6";
const std::string PaladinForwardWalkOverlay = "/data/global/ui/FrontEnd/paladin/PAFWs.DC6";
const std::string PaladinBackWalk = "/data/global/ui/FrontEnd/paladin/PABW.DC6";

const std::string AmazonUnselected = "/data/global/ui/FrontEnd/amazon/AMNU1.DC6";
const std::string AmazonUnselectedH = "/data/global/ui/FrontEnd/amazon/AMNU2.DC6";
const std::string AmazonSelected = "/data/global/ui/FrontEnd/amazon/AMNU3.DC6";
const std::string AmazonForwardWalk = "/data/global/ui/FrontEnd/amazon/AMFW.DC6";
const std::string AmazonForwardWalkOverlay = "/data/global/ui/FrontEnd/amazon/AMFWs.DC6";
const std::string AmazonBackWalk = "/data/global/ui/FrontEnd/amazon/AMBW.DC6";

const std::string AssassinUnselected = "/data/global/ui/FrontEnd/assassin/ASNU1.DC6";
const std::string AssassinUnselectedH = "/data/global/ui/FrontEnd/assassin/ASNU2.DC6";
const std::string AssassinSelected = "/data/global/ui/FrontEnd/assassin/ASNU3.DC6";
const std::string AssassinForwardWalk = "/data/global/ui/FrontEnd/assassin/ASFW.DC6";
const std::string AssassinBackWalk = "/data/global/ui/FrontEnd/assassin/ASBW.DC6";

const std::string DruidUnselected = "/data/global/ui/FrontEnd/druid/DZNU1.dc6";
const std::string DruidUnselectedH = "/data/global/ui/FrontEnd/druid/DZNU2.dc6";
const std::string DruidSelected = "/data/global/ui/FrontEnd/druid/DZNU3.DC6";
const std::string DruidForwardWalk = "/data/global/ui/FrontEnd/druid/DZFW.DC6";
const std::string DruidBackWalk = "/data/global/ui/FrontEnd/druid/DZBW.DC6";
} // namespace CharacterSelectScreen

namespace CharacterSelection {
const std::string Background = "/data/global/ui/CharSelect/characterselectscreenEXP.dc6";
const std::string SelectBox = "/data/global/ui/CharSelect/charselectbox.dc6";
const std::string PopUpOkCancel = "/data/global/ui/FrontEnd/PopUpOKCancel.dc6";
} // namespace CharacterSelection

namespace Game {
const std::string Panels = "/data/global/ui/PANEL/800ctrlpnl7.dc6";
const std::string GlobeOverlap = "/data/global/ui/PANEL/overlap.DC6";
const std::string HealthManaIndicator = "/data/global/ui/PANEL/hlthmana.DC6";
const std::string AddSkillButton = "/data/global/ui/PANEL/level.DC6";
const std::string MoveGoldDialog = "/data/global/ui/menu/dialogbackground.DC6";
const std::string WPTabs = "/data/global/ui/menu/expwaygatetabs.dc6";
const std::string WPBg = "/data/global/ui/menu/waygatebackground.dc6";
const std::string WPIcons = "/data/global/ui/menu/waygateicons.dc6";
const std::string UpDownArrows = "/data/global/ui/BIGMENU/numberarrows.dc6";
} // namespace Game

// --- Escape Menu ---
namespace EscapeMenu {
namespace Main {
const std::string Options = "/data/local/ui/" + Language::LanguageTableToken + "/options.dc6";
const std::string Exit = "/data/local/ui/" + Language::LanguageTableToken + "/exit.dc6";
const std::string ReturnToGame = "/data/local/ui/" + Language::LanguageTableToken + "/returntogame.dc6";
} // namespace Main
namespace Options {
const std::string SoundOptions = "/data/local/ui/" + Language::LanguageTableToken + "/soundoptions.dc6";
const std::string VideoOptions = "/data/local/ui/" + Language::LanguageTableToken + "/videoOptions.dc6";
const std::string AutoMapOptions = "/data/local/ui/" + Language::LanguageTableToken + "/automapOptions.dc6";
const std::string CfgOptions = "/data/local/ui/" + Language::LanguageTableToken + "/cfgOptions.dc6";
const std::string Previous = "/data/local/ui/" + Language::LanguageTableToken + "/previous.dc6";
} // namespace Options
} // namespace EscapeMenu

// sound options
const std::string EscapeSndOptSoundVolume = "/data/local/ui/" + Language::LanguageTableToken + "/sound.dc6";
const std::string EscapeSndOptMusicVolume = "/data/local/ui/" + Language::LanguageTableToken + "/music.dc6";
const std::string EscapeSndOpt3DBias = "/data/local/ui/" + Language::LanguageTableToken + "/3dbias.dc6";
const std::string EscapeSndOptNPCSpeech = "/data/local/ui/" + Language::LanguageTableToken + "/npcspeech.dc6";
const std::string EscapeSndOptNPCSpeechAudioAndText = "/data/local/ui/" + Language::LanguageTableToken + "/audiotext.dc6";
const std::string EscapeSndOptNPCSpeechAudioOnly = "/data/local/ui/" + Language::LanguageTableToken + "/audioonly.dc6";
const std::string EscapeSndOptNPCSpeechTextOnly = "/data/local/ui/" + Language::LanguageTableToken + "/textonly.dc6";

// video options
const std::string EscapeVidOptRes = "/data/local/ui/" + Language::LanguageTableToken + "/resolution.dc6";
const std::string EscapeVidOptLightQuality = "/data/local/ui/" + Language::LanguageTableToken + "/lightquality.dc6";
const std::string EscapeVidOptBlendShadow = "/data/local/ui/" + Language::LanguageTableToken + "/blendshadow.dc6";
const std::string EscapeVidOptPerspective = "/data/local/ui/" + Language::LanguageTableToken + "/prespective.dc6";
const std::string EscapeVidOptGamma = "/data/local/ui/" + Language::LanguageTableToken + "/gamma.dc6";
const std::string EscapeVidOptContrast = "/data/local/ui/" + Language::LanguageTableToken + "/contrast.dc6";

// auto map
const std::string EscapeAutoMapOptSize = "/data/local/ui/" + Language::LanguageTableToken + "/automapmode.dc6";
const std::string EscapeAutoMapOptFade = "/data/local/ui/" + Language::LanguageTableToken + "/automapfade.dc6";
const std::string EscapeAutoMapOptCenter = "/data/local/ui/" + Language::LanguageTableToken + "/automapcenter.dc6";
const std::string EscapeAutoMapOptNames = "/data/local/ui/" + Language::LanguageTableToken + "/automappartynames.dc6";

// automap size
const std::string EscapeAutoMapOptFullScreen = "/data/local/ui/" + Language::LanguageTableToken + "/full.dc6";
const std::string EscapeAutoMapOptMiniMap = "/data/local/ui/" + Language::LanguageTableToken + "/mini.dc6";

// resolutions
const std::string EscapeVideoOptRes640x480 = "/data/local/ui/" + Language::LanguageTableToken + "/640x480.dc6";
const std::string EscapeVideoOptRes800x600 = "/data/local/ui/" + Language::LanguageTableToken + "/800x800.dc6";

const std::string EscapeOn = "/data/local/ui/" + Language::LanguageTableToken + "/smallon.dc6";
const std::string EscapeOff = "/data/local/ui/" + Language::LanguageTableToken + "/smalloff.dc6";
const std::string EscapeYes = "/data/local/ui/" + Language::LanguageTableToken + "/smallyes.dc6";
const std::string EscapeNo = "/data/local/ui/" + Language::LanguageTableToken + "/smallno.dc6";
const std::string EscapeSlideBar = "/data/global/ui/widgets/optbarc.dc6";
const std::string EscapeSlideBarSkull = "/data/global/ui/widgets/optskull.dc6";

// --- Help Overlay ---

const std::string HelpBorder = "/data/global/ui/MENU/800helpborder.DC6";
const std::string HelpYellowBullet = "/data/global/ui/MENU/helpyellowbullet.DC6";
const std::string HelpWhiteBullet = "/data/global/ui/MENU/helpwhitebullet.DC6";

// Box pieces, used in all in game boxes like npc interaction menu on click, the chat window and the key binding menu
const std::string BoxPieces = "/data/global/ui/MENU/boxpieces.DC6";

// TextSlider contains the pieces to build a scrollbar in the menus, such as the one in the configure keys menu
const std::string TextSlider = "/data/global/ui/MENU/textslid.DC6";

// Issue #685 - used in the mini-panel
const std::string GameSmallMenuButton = "/data/global/ui/PANEL/menubutton.DC6";
const std::string SkillIcon = "/data/global/ui/PANEL/Skillicon.DC6";

namespace QuestLog {
const std::string QuestLogBg = "/data/global/ui/MENU/questbackground.dc6";
const std::string QuestLogDone = "/data/global/ui/MENU/questdone.dc6";
const std::string QuestLogTabs = "/data/global/ui/MENU/expquesttabs.dc6";
const std::string QuestLogQDescrBtn = "/data/global/ui/MENU/questlast.dc6";
const std::string QuestLogSocket = "/data/global/ui/MENU/questsockets.dc6";
const std::string QuestLogAQuestAnimation = "/data/global/ui/MENU/a%dq%d.dc6";
const std::string QuestLogDoneSfx = "cursor/questdone.wav";
} // namespace QuestLog

namespace MousePointers {
const std::string CursorDefault = "/data/global/ui/CURSOR/ohand.DC6";
}

namespace FontsAndLocales {
const std::string Font6 = "/data/local/FONT/" + Language::LanguageFontToken + "/font6";
const std::string Font8 = "/data/local/FONT/" + Language::LanguageFontToken + "/font8";
const std::string Font16 = "/data/local/FONT/" + Language::LanguageFontToken + "/font16";
const std::string Font24 = "/data/local/FONT/" + Language::LanguageFontToken + "/font24";
const std::string Font30 = "/data/local/FONT/" + Language::LanguageFontToken + "/font30";
const std::string Font42 = "/data/local/FONT/" + Language::LanguageFontToken + "/font42";
const std::string FontFormal12 = "/data/local/FONT/" + Language::LanguageFontToken + "/fontformal12";
const std::string FontFormal11 = "/data/local/FONT/" + Language::LanguageFontToken + "/fontformal11";
const std::string FontFormal10 = "/data/local/FONT/" + Language::LanguageFontToken + "/fontformal10";
const std::string FontExocet10 = "/data/local/FONT/" + Language::LanguageFontToken + "/fontexocet10";
const std::string FontExocet8 = "/data/local/FONT/" + Language::LanguageFontToken + "/fontexocet8";
const std::string FontSucker = "/data/local/FONT/" + Language::LanguageFontToken + "/ReallyTheLastSucker";
const std::string FontRediculous = "/data/local/FONT/" + Language::LanguageFontToken + "/fontridiculous";
const std::string ExpansionStringTable = "/data/local/lng/" + Language::LanguageTableToken + "/expansionstring.tbl";
const std::string StringTable = "/data/local/lng/" + Language::LanguageTableToken + "/string.tbl";
const std::string PatchStringTable = "/data/local/lng/" + Language::LanguageTableToken + "/patchstring.tbl";
} // namespace FontsAndLocales

namespace UI {
const std::string WideButtonBlank = "/data/global/ui/FrontEnd/WideButtonBlank.dc6";
const std::string MediumButtonBlank = "/data/global/ui/FrontEnd/MediumButtonBlank.dc6";
const std::string CancelButton = "/data/global/ui/FrontEnd/CancelButtonBlank.dc6";
const std::string NarrowButtonBlank = "/data/global/ui/FrontEnd/NarrowButtonBlank.dc6";
const std::string ShortButtonBlank = "/data/global/ui/CharSelect/ShortButtonBlank.dc6";
const std::string TextBox2 = "/data/global/ui/FrontEnd/textbox2.dc6";
const std::string TallButtonBlank = "/data/global/ui/CharSelect/TallButtonBlank.dc6";
const std::string Checkbox = "/data/global/ui/FrontEnd/clickbox.dc6";
const std::string Scrollbar = "/data/global/ui/PANEL/scrollbar.dc6";

const std::string PopUpLarge = "/data/global/ui/FrontEnd/PopUpLarge.dc6";
const std::string PopUpLargest = "/data/global/ui/FrontEnd/PopUpLargest.dc6";
const std::string PopUpWide = "/data/global/ui/FrontEnd/PopUpWide.dc6";
const std::string PopUpOk = "/data/global/ui/FrontEnd/PopUpOk.dc6";
const std::string PopUpOk2 = "/data/global/ui/FrontEnd/PopUpOk.dc6";
const std::string PopUpOkCancel2 = "/data/global/ui/FrontEnd/PopUpOkCancel2.dc6";
const std::string PopUp340x224 = "/data/global/ui/FrontEnd/PopUp_340x224.dc6";
} // namespace UI

namespace GameUI {
const std::string PentSpin = "/data/global/ui/CURSOR/pentspin.DC6";
const std::string Minipanel = "/data/global/ui/PANEL/minipanel.DC6";
const std::string MinipanelSmall = "/data/global/ui/PANEL/minipanel_s.dc6";
const std::string MinipanelButton = "/data/global/ui/PANEL/minipanelbtn.DC6";

const std::string Frame = "/data/global/ui/PANEL/800borderframe.dc6";
const std::string InventoryCharacterPanel = "/data/global/ui/PANEL/invchar6.DC6";
const std::string PartyPanel = "/data/global/ui/MENU/party.dc6";
const std::string PartyButton = "/data/global/ui/MENU/partybuttons.dc6";
const std::string PartyBoxes = "/data/global/ui/MENU/partyboxes.dc6";
const std::string PartyBar = "/data/global/ui/MENU/partybar.dc6";
const std::string HeroStatsPanelStatsPoints = "/data/global/ui/PANEL/skillpoints.dc6";
const std::string HeroStatsPanelSocket = "/data/global/ui/PANEL/levelsocket.dc6";
const std::string InventoryWeaponsTab = "/data/global/ui/PANEL/invchar6Tab.DC6";
const std::string SkillsPanelAmazon = "/data/global/ui/SPELLS/skltree_a_back.DC6";
const std::string SkillsPanelBarbarian = "/data/global/ui/SPELLS/skltree_b_back.DC6";
const std::string SkillsPanelDruid = "/data/global/ui/SPELLS/skltree_d_back.DC6";
const std::string SkillsPanelAssassin = "/data/global/ui/SPELLS/skltree_i_back.DC6";
const std::string SkillsPanelNecromancer = "/data/global/ui/SPELLS/skltree_n_back.DC6";
const std::string SkillsPanelPaladin = "/data/global/ui/SPELLS/skltree_p_back.DC6";
const std::string SkillsPanelSorcerer = "/data/global/ui/SPELLS/skltree_s_back.DC6";

const std::string GenericSkills = "/data/global/ui/SPELLS/Skillicon.DC6";
const std::string AmazonSkills = "/data/global/ui/SPELLS/AmSkillicon.DC6";
const std::string BarbarianSkills = "/data/global/ui/SPELLS/BaSkillicon.DC6";
const std::string DruidSkills = "/data/global/ui/SPELLS/DrSkillicon.DC6";
const std::string AssassinSkills = "/data/global/ui/SPELLS/AsSkillicon.DC6";
const std::string NecromancerSkills = "/data/global/ui/SPELLS/NeSkillicon.DC6";
const std::string PaladinSkills = "/data/global/ui/SPELLS/PaSkillicon.DC6";
const std::string SorcererSkills = "/data/global/ui/SPELLS/SoSkillicon.DC6";

const std::string RunButton = "/data/global/ui/PANEL/runbutton.dc6";
const std::string MenuButton = "/data/global/ui/PANEL/menubutton.DC6";
const std::string GoldCoinButton = "/data/global/ui/panel/goldcoinbtn.dc6";
const std::string BuySellButton = "/data/global/ui/panel/buysellbtn.dc6";

const std::string ArmorPlaceholder = "/data/global/ui/PANEL/inv_armor.DC6";
const std::string BeltPlaceholder = "/data/global/ui/PANEL/inv_belt.DC6";
const std::string BootsPlaceholder = "/data/global/ui/PANEL/inv_boots.DC6";
const std::string HelmGlovePlaceholder = "/data/global/ui/PANEL/inv_helm_glove.DC6";
const std::string RingAmuletPlaceholder = "/data/global/ui/PANEL/inv_ring_amulet.DC6";
const std::string WeaponsPlaceholder = "/data/global/ui/PANEL/inv_weapons.DC6";
} // namespace GameUI

namespace Data {
const std::string LevelPreset = "/data/global/excel/LvlPrest.txt";
const std::string LevelType = "/data/global/excel/LvlTypes.txt";
const std::string ObjectType = "/data/global/excel/objtype.txt";
const std::string LevelWarp = "/data/global/excel/LvlWarp.txt";
const std::string LevelDetails = "/data/global/excel/Levels.txt";
const std::string LevelMaze = "/data/global/excel/LvlMaze.txt";
const std::string LevelSubstitutions = "/data/global/excel/LvlSub.txt";

const std::string ObjectDetails = "/data/global/excel/Objects.txt";
const std::string ObjectMode = "/data/global/excel/ObjMode.txt";
const std::string SoundSettings = "/data/global/excel/Sounds.txt";
const std::string ItemStatCost = "/data/global/excel/ItemStatCost.txt";
const std::string ItemRatio = "/data/global/excel/itemratio.txt";
const std::string ItemTypes = "/data/global/excel/ItemTypes.txt";
const std::string QualityItems = "/data/global/excel/qualityitems.txt";
const std::string LowQualityItems = "/data/global/excel/lowqualityitems.txt";
const std::string Overlays = "/data/global/excel/Overlay.txt";
const std::string Runes = "/data/global/excel/runes.txt";
const std::string Sets = "/data/global/excel/Sets.txt";
const std::string SetItems = "/data/global/excel/SetItems.txt";
const std::string AutoMagic = "/data/global/excel/automagic.txt";
const std::string BodyLocations = "/data/global/excel/bodylocs.txt";
const std::string Events = "/data/global/excel/events.txt";
const std::string Properties = "/data/global/excel/Properties.txt";
const std::string Hireling = "/data/global/excel/hireling.txt";
const std::string HirelingDescription = "/data/global/excel/HireDesc.txt";
const std::string DifficultyLevels = "/data/global/excel/difficultylevels.txt";
const std::string AutoMap = "/data/global/excel/AutoMap.txt";
const std::string CubeRecipes = "/data/global/excel/cubemain.txt";
const std::string CubeModifier = "/data/global/excel/CubeMod.txt";
const std::string CubeType = "/data/global/excel/CubeType.txt";
const std::string Skills = "/data/global/excel/skills.txt";
const std::string SkillDesc = "/data/global/excel/skilldesc.txt";
const std::string SkillCalc = "/data/global/excel/skillcalc.txt";
const std::string MissileCalc = "/data/global/excel/misscalc.txt";
const std::string TreasureClass = "/data/global/excel/TreasureClass.txt";
const std::string TreasureClassEx = "/data/global/excel/TreasureClassEx.txt";
const std::string States = "/data/global/excel/states.txt";
const std::string SoundEnvirons = "/data/global/excel/soundenviron.txt";
const std::string Shrines = "/data/global/excel/shrines.txt";
const std::string MonProp = "/data/global/excel/Monprop.txt";
const std::string ElemType = "/data/global/excel/ElemTypes.txt";
const std::string PlrMode = "/data/global/excel/PlrMode.txt";
const std::string PetType = "/data/global/excel/pettype.txt";
const std::string NPC = "/data/global/excel/npc.txt";
const std::string MonsterUniqueModifier = "/data/global/excel/monumod.txt";
const std::string MonsterEquipment = "/data/global/excel/monequip.txt";
const std::string UniqueAppellation = "/data/global/excel/UniqueAppellation.txt";
const std::string MonsterLevel = "/data/global/excel/monlvl.txt";
const std::string MonsterSound = "/data/global/excel/monsounds.txt";
const std::string MonsterSequence = "/data/global/excel/monseq.txt";
const std::string PlayerClass = "/data/global/excel/PlayerClass.txt";
const std::string PlayerType = "/data/global/excel/PlrType.txt";
const std::string Composite = "/data/global/excel/Composit.txt";
const std::string HitClass = "/data/global/excel/HitClass.txt";
const std::string ObjectGroup = "/data/global/excel/objgroup.txt";
const std::string CompCode = "/data/global/excel/compcode.txt";
const std::string Belts = "/data/global/excel/belts.txt";
const std::string Gamble = "/data/global/excel/gamble.txt";
const std::string Colors = "/data/global/excel/colors.txt";
const std::string StorePage = "/data/global/excel/StorePage.txt";
} // namespace Data

namespace Animations {
const std::string ObjectData = "/data/global/objects";
const std::string AnimationData = "/data/global/animdata.d2";
const std::string PlayerAnimationBase = "/data/global/CHARS";
const std::string MissileData = "/data/global/missiles";
const std::string ItemGraphics = "/data/global/items";
} // namespace Animations

namespace InventoryData {
const std::string Inventory = "/data/global/excel/inventory.txt";
const std::string Weapons = "/data/global/excel/weapons.txt";
const std::string Armor = "/data/global/excel/armor.txt";
const std::string ArmorType = "/data/global/excel/ArmType.txt";
const std::string WeaponClass = "/data/global/excel/WeaponClass.txt";
const std::string Books = "/data/global/excel/books.txt";
const std::string Misc = "/data/global/excel/misc.txt";
const std::string UniqueItems = "/data/global/excel/UniqueItems.txt";
const std::string Gems = "/data/global/excel/gems.txt";
} // namespace InventoryData

namespace Affixes {
const std::string MagicPrefix = "/data/global/excel/MagicPrefix.txt";
const std::string MagicSuffix = "/data/global/excel/MagicSuffix.txt";
const std::string RarePrefix = "/data/global/excel/RarePrefix.txt"; // these are for item names
const std::string RareSuffix = "/data/global/excel/RareSuffix.txt";
} // namespace Affixes

namespace MonsterPrefixSuffixes {
const std::string UniquePrefix = "/data/global/excel/UniquePrefix.txt";
const std::string UniqueSuffix = "/data/global/excel/UniqueSuffix.txt";
} // namespace MonsterPrefixSuffixes

namespace CharacterData {
const std::string Experience = "/data/global/excel/experience.txt";
const std::string CharStats = "/data/global/excel/charstats.txt";
} // namespace CharacterData

namespace Music {
const std::string Title = "/data/global/music/introedit.wav";
const std::string Options = "/data/global/music/Common/options.wav";
const std::string Act1AndarielAction = "/data/global/music/Act1/andarielaction.wav";
const std::string Act1BloodRavenResolution = "/data/global/music/Act1/bloodravenresolution.wav";
const std::string Act1Caves = "/data/global/music/Act1/caves.wav";
const std::string Act1Crypt = "/data/global/music/Act1/crypt.wav";
const std::string Act1DenOfEvilAction = "/data/global/music/Act1/denofevilaction.wav";
const std::string Act1Monastery = "/data/global/music/Act1/monastery.wav";
const std::string Act1Town1 = "/data/global/music/Act1/town1.wav";
const std::string Act1Tristram = "/data/global/music/Act1/tristram.wav";
const std::string Act1Wild = "/data/global/music/Act1/wild.wav";
const std::string Act2Desert = "/data/global/music/Act2/desert.wav";
const std::string Act2Harem = "/data/global/music/Act2/harem.wav";
const std::string Act2HoradricAction = "/data/global/music/Act2/horadricaction.wav";
const std::string Act2Lair = "/data/global/music/Act2/lair.wav";
const std::string Act2RadamentResolution = "/data/global/music/Act2/radamentresolution.wav";
const std::string Act2Sanctuary = "/data/global/music/Act2/sanctuary.wav";
const std::string Act2Sewer = "/data/global/music/Act2/sewer.wav";
const std::string Act2TaintedSunAction = "/data/global/music/Act2/taintedsunaction.wav";
const std::string Act2Tombs = "/data/global/music/Act2/tombs.wav";
const std::string Act2Town2 = "/data/global/music/Act2/town2.wav";
const std::string Act2Valley = "/data/global/music/Act2/valley.wav";
const std::string Act3Jungle = "/data/global/music/Act3/jungle.wav";
const std::string Act3Kurast = "/data/global/music/Act3/kurast.wav";
const std::string Act3KurastSewer = "/data/global/music/Act3/kurastsewer.wav";
const std::string Act3MefDeathAction = "/data/global/music/Act3/mefdeathaction.wav";
const std::string Act3OrbAction = "/data/global/music/Act3/orbaction.wav";
const std::string Act3Spider = "/data/global/music/Act3/spider.wav";
const std::string Act3Town3 = "/data/global/music/Act3/town3.wav";
const std::string Act4Diablo = "/data/global/music/Act4/diablo.wav";
const std::string Act4DiabloAction = "/data/global/music/Act4/diabloaction.wav";
const std::string Act4ForgeAction = "/data/global/music/Act4/forgeaction.wav";
const std::string Act4IzualAction = "/data/global/music/Act4/izualaction.wav";
const std::string Act4Mesa = "/data/global/music/Act4/mesa.wav";
const std::string Act4Town4 = "/data/global/music/Act4/town4.wav";
const std::string Act5Baal = "/data/global/music/Act5/baal.wav";
const std::string Act5Siege = "/data/global/music/Act5/siege.wav";
const std::string Act5Shenk = "/data/global/music/Act5/shenkmusic.wav";
const std::string Act5XTown = "/data/global/music/Act5/xtown.wav";
const std::string Act5XTemple = "/data/global/music/Act5/xtemple.wav";
const std::string Act5IceCaves = "/data/global/music/Act5/icecaves.wav";
const std::string Act5Nihlathak = "/data/global/music/Act5/nihlathakmusic.wav";
} // namespace Music

namespace SFX {
const std::string CursorSelect = "cursor_select";
const std::string ButtonClick = "/data/global/sfx/cursor/button.wav";
const std::string AmazonDeselect = "cursor_amazon_deselect";
const std::string AmazonSelect = "cursor_amazon_select";
const std::string AssassinDeselect = "Cursor/intro/assassin deselect.wav";
const std::string AssassinSelect = "Cursor/intro/assassin select.wav";
const std::string BarbarianDeselect = "cursor_barbarian_deselect";
const std::string BarbarianSelect = "cursor_barbarian_select";
const std::string DruidDeselect = "Cursor/intro/druid deselect.wav";
const std::string DruidSelect = "Cursor/intro/druid select.wav";
const std::string NecromancerDeselect = "cursor_necromancer_deselect";
const std::string NecromancerSelect = "cursor_necromancer_select";
const std::string PaladinDeselect = "cursor_paladin_deselect";
const std::string PaladinSelect = "cursor_paladin_select";
const std::string SorceressDeselect = "cursor_sorceress_deselect";
const std::string SorceressSelect = "cursor_sorceress_select";
} // namespace SFX

// --- Enemy Data ---
namespace EnemyData {
const std::string MonStats = "/data/global/excel/monstats.txt";
const std::string MonStats2 = "/data/global/excel/monstats2.txt";
const std::string MonPreset = "/data/global/excel/monpreset.txt";
const std::string MonType = "/data/global/excel/Montype.txt";
const std::string SuperUniques = "/data/global/excel/SuperUniques.txt";
const std::string MonMode = "/data/global/excel/monmode.txt";
const std::string MonsterPlacement = "/data/global/excel/MonPlace.txt";
const std::string MonsterAI = "/data/global/excel/monai.txt";
} // namespace EnemyData

namespace SkillData {
const std::string Missiles = "/data/global/excel/Missiles.txt";
}

namespace Palettes {
const std::string Act1 = "/data/global/palette/act1/pal.dat";
const std::string Act2 = "/data/global/palette/act2/pal.dat";
const std::string Act3 = "/data/global/palette/act3/pal.dat";
const std::string Act4 = "/data/global/palette/act4/pal.dat";
const std::string Act5 = "/data/global/palette/act5/pal.dat";
const std::string EndGame = "/data/global/palette/endgame/pal.dat";
const std::string EndGame2 = "/data/global/palette/endgame2/pal.dat";
const std::string Fechar = "/data/global/palette/fechar/pal.dat";
const std::string Loading = "/data/global/palette/loading/pal.dat";
const std::string Menu0 = "/data/global/palette/menu0/pal.dat";
const std::string Menu1 = "/data/global/palette/menu1/pal.dat";
const std::string Menu2 = "/data/global/palette/menu2/pal.dat";
const std::string Menu3 = "/data/global/palette/menu3/pal.dat";
const std::string Menu4 = "/data/global/palette/menu4/pal.dat";
const std::string Sky = "/data/global/palette/sky/pal.dat";
const std::string Static = "/data/global/palette/static/pal.dat";
const std::string Trademark = "/data/global/palette/trademark/pal.dat";
const std::string Units = "/data/global/palette/units/pal.dat";
} // namespace Palettes

namespace PaletteTransforms {
const std::string Act1 = "/data/global/palette/act1/Pal.pl2";
const std::string Act2 = "/data/global/palette/act2/Pal.pl2";
const std::string Act3 = "/data/global/palette/act3/Pal.pl2";
const std::string Act4 = "/data/global/palette/act4/Pal.pl2";
const std::string Act5 = "/data/global/palette/act5/Pal.pl2";
const std::string EndGame = "/data/global/palette/endgame/Pal.pl2";
const std::string EndGame2 = "/data/global/palette/endgame2/Pal.pl2";
const std::string Fechar = "/data/global/palette/fechar/Pal.pl2";
const std::string Loading = "/data/global/palette/loading/Pal.pl2";
const std::string Menu0 = "/data/global/palette/menu0/Pal.pl2";
const std::string Menu1 = "/data/global/palette/menu1/Pal.pl2";
const std::string Menu2 = "/data/global/palette/menu2/Pal.pl2";
const std::string Menu3 = "/data/global/palette/menu3/Pal.pl2";
const std::string Menu4 = "/data/global/palette/menu4/Pal.pl2";
const std::string Sky = "/data/global/palette/sky/Pal.pl2";
const std::string Trademark = "/data/global/palette/trademark/Pal.pl2";
} // namespace PaletteTransforms
} // namespace OD2::Common::ResourcePaths
