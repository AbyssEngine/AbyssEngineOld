#pragma once

#include <string_view>

namespace OD2::Common::ResourcePaths {

#define OD2_LANGUAGE_TABLE_TOKEN "{LANG}"
#define OD2_LANGUAGE_FONT_TOKEN "{LANG_FONT}"

namespace Language {
inline constexpr std::string_view LocalLanguage = "/data/local/use";
inline constexpr std::string_view LanguageFontToken = OD2_LANGUAGE_FONT_TOKEN;
inline constexpr std::string_view LanguageTableToken = OD2_LANGUAGE_TABLE_TOKEN;
} // namespace Language

namespace Screens {
inline constexpr std::string_view LoadingScreen = "/data/global/ui/Loading/loadingscreen.dc6";
}

namespace MainMenu {
inline constexpr std::string_view TrademarkScreen = "/data/global/ui/FrontEnd/trademarkscreenEXP.dc6";
inline constexpr std::string_view GameSelectScreen = "/data/global/ui/FrontEnd/gameselectscreenEXP.dc6";
inline constexpr std::string_view TCPIPBackground = "/data/global/ui/FrontEnd/TCPIPscreen.dc6";
inline constexpr std::string_view Diablo2LogoFireLeft = "/data/global/ui/FrontEnd/D2logoFireLeft.DC6";
inline constexpr std::string_view Diablo2LogoFireRight = "/data/global/ui/FrontEnd/D2logoFireRight.DC6";
inline constexpr std::string_view Diablo2LogoBlackLeft = "/data/global/ui/FrontEnd/D2logoBlackLeft.DC6";
inline constexpr std::string_view Diablo2LogoBlackRight = "/data/global/ui/FrontEnd/D2logoBlackRight.DC6";
} // namespace MainMenu

namespace Credits {
inline constexpr std::string_view CreditsBackground = "/data/global/ui/CharSelect/creditsbckgexpand.dc6";
inline constexpr std::string_view CreditsText = "/data/local/ui/" OD2_LANGUAGE_TABLE_TOKEN "/ExpansionCredits.txt";
} // namespace Credits

namespace Cinematics {
inline constexpr std::string_view Background = "/data/global/ui/FrontEnd/CinematicsSelectionEXP.dc6";
}

namespace Videos {
inline constexpr std::string_view BlizardStartup1 = "/data/local/video/New_Bliz640x480.bik";
inline constexpr std::string_view BlizardStartup2 = "/data/local/video/BlizNorth640x480.bik";
inline constexpr std::string_view Act1Intro = "/data/local/video/" OD2_LANGUAGE_TABLE_TOKEN "/d2intro640x292.bik";
inline constexpr std::string_view Act2Intro = "/data/local/video/" OD2_LANGUAGE_TABLE_TOKEN "/act02start640x292.bik";
inline constexpr std::string_view Act3Intro = "/data/local/video/" OD2_LANGUAGE_TABLE_TOKEN "/act03start640x292.bik";
inline constexpr std::string_view Act4Intro = "/data/local/video/" OD2_LANGUAGE_TABLE_TOKEN "/act04start640x292.bik";
inline constexpr std::string_view Act4Outro = "/data/local/video/" OD2_LANGUAGE_TABLE_TOKEN "/act04end640x292.bik";
inline constexpr std::string_view Act5Intro = "/data/local/video/" OD2_LANGUAGE_TABLE_TOKEN "/d2x_intro_640x292.bik";
inline constexpr std::string_view Act5Outro = "/data/local/video/" OD2_LANGUAGE_TABLE_TOKEN "/d2x_out_640x292.bik";
} // namespace Videos

namespace CharacterSelectScreen {
inline constexpr std::string_view Background = "/data/global/ui/FrontEnd/charactercreationscreenEXP.dc6";
inline constexpr std::string_view Campfire = "/data/global/ui/FrontEnd/fire.DC6";

inline constexpr std::string_view BarbarianUnselected = "/data/global/ui/FrontEnd/barbarian/banu1.DC6";
inline constexpr std::string_view BarbarianUnselectedH = "/data/global/ui/FrontEnd/barbarian/banu2.DC6";
inline constexpr std::string_view BarbarianSelected = "/data/global/ui/FrontEnd/barbarian/banu3.DC6";
inline constexpr std::string_view BarbarianForwardWalk = "/data/global/ui/FrontEnd/barbarian/bafw.DC6";
inline constexpr std::string_view BarbarianForwardWalkOverlay = "/data/global/ui/FrontEnd/barbarian/BAFWs.DC6";
inline constexpr std::string_view BarbarianBackWalk = "/data/global/ui/FrontEnd/barbarian/babw.DC6";

inline constexpr std::string_view SorceressUnselected = "/data/global/ui/FrontEnd/sorceress/SONU1.DC6";
inline constexpr std::string_view SorceressUnselectedH = "/data/global/ui/FrontEnd/sorceress/SONU2.DC6";
inline constexpr std::string_view SorceressSelected = "/data/global/ui/FrontEnd/sorceress/SONU3.DC6";
inline constexpr std::string_view SorceressSelectedOverlay = "/data/global/ui/FrontEnd/sorceress/SONU3s.DC6";
inline constexpr std::string_view SorceressForwardWalk = "/data/global/ui/FrontEnd/sorceress/SOFW.DC6";
inline constexpr std::string_view SorceressForwardWalkOverlay = "/data/global/ui/FrontEnd/sorceress/SOFWs.DC6";
inline constexpr std::string_view SorceressBackWalk = "/data/global/ui/FrontEnd/sorceress/SOBW.DC6";
inline constexpr std::string_view SorceressBackWalkOverlay = "/data/global/ui/FrontEnd/sorceress/SOBWs.DC6";

inline constexpr std::string_view NecromancerUnselected = "/data/global/ui/FrontEnd/necromancer/NENU1.DC6";
inline constexpr std::string_view NecromancerUnselectedH = "/data/global/ui/FrontEnd/necromancer/NENU2.DC6";
inline constexpr std::string_view NecromancerSelected = "/data/global/ui/FrontEnd/necromancer/NENU3.DC6";
inline constexpr std::string_view NecromancerSelectedOverlay = "/data/global/ui/FrontEnd/necromancer/NENU3s.DC6";
inline constexpr std::string_view NecromancerForwardWalk = "/data/global/ui/FrontEnd/necromancer/NEFW.DC6";
inline constexpr std::string_view NecromancerForwardWalkOverlay = "/data/global/ui/FrontEnd/necromancer/NEFWs.DC6";
inline constexpr std::string_view NecromancerBackWalk = "/data/global/ui/FrontEnd/necromancer/NEBW.DC6";
inline constexpr std::string_view NecromancerBackWalkOverlay = "/data/global/ui/FrontEnd/necromancer/NEBWs.DC6";

inline constexpr std::string_view PaladinUnselected = "/data/global/ui/FrontEnd/paladin/PANU1.DC6";
inline constexpr std::string_view PaladinUnselectedH = "/data/global/ui/FrontEnd/paladin/PANU2.DC6";
inline constexpr std::string_view PaladinSelected = "/data/global/ui/FrontEnd/paladin/PANU3.DC6";
inline constexpr std::string_view PaladinForwardWalk = "/data/global/ui/FrontEnd/paladin/PAFW.DC6";
inline constexpr std::string_view PaladinForwardWalkOverlay = "/data/global/ui/FrontEnd/paladin/PAFWs.DC6";
inline constexpr std::string_view PaladinBackWalk = "/data/global/ui/FrontEnd/paladin/PABW.DC6";

inline constexpr std::string_view AmazonUnselected = "/data/global/ui/FrontEnd/amazon/AMNU1.DC6";
inline constexpr std::string_view AmazonUnselectedH = "/data/global/ui/FrontEnd/amazon/AMNU2.DC6";
inline constexpr std::string_view AmazonSelected = "/data/global/ui/FrontEnd/amazon/AMNU3.DC6";
inline constexpr std::string_view AmazonForwardWalk = "/data/global/ui/FrontEnd/amazon/AMFW.DC6";
inline constexpr std::string_view AmazonForwardWalkOverlay = "/data/global/ui/FrontEnd/amazon/AMFWs.DC6";
inline constexpr std::string_view AmazonBackWalk = "/data/global/ui/FrontEnd/amazon/AMBW.DC6";

inline constexpr std::string_view AssassinUnselected = "/data/global/ui/FrontEnd/assassin/ASNU1.DC6";
inline constexpr std::string_view AssassinUnselectedH = "/data/global/ui/FrontEnd/assassin/ASNU2.DC6";
inline constexpr std::string_view AssassinSelected = "/data/global/ui/FrontEnd/assassin/ASNU3.DC6";
inline constexpr std::string_view AssassinForwardWalk = "/data/global/ui/FrontEnd/assassin/ASFW.DC6";
inline constexpr std::string_view AssassinBackWalk = "/data/global/ui/FrontEnd/assassin/ASBW.DC6";

inline constexpr std::string_view DruidUnselected = "/data/global/ui/FrontEnd/druid/DZNU1.dc6";
inline constexpr std::string_view DruidUnselectedH = "/data/global/ui/FrontEnd/druid/DZNU2.dc6";
inline constexpr std::string_view DruidSelected = "/data/global/ui/FrontEnd/druid/DZNU3.DC6";
inline constexpr std::string_view DruidForwardWalk = "/data/global/ui/FrontEnd/druid/DZFW.DC6";
inline constexpr std::string_view DruidBackWalk = "/data/global/ui/FrontEnd/druid/DZBW.DC6";
} // namespace CharacterSelectScreen

namespace CharacterSelection {
inline constexpr std::string_view Background = "/data/global/ui/CharSelect/characterselectscreenEXP.dc6";
inline constexpr std::string_view SelectBox = "/data/global/ui/CharSelect/charselectbox.dc6";
inline constexpr std::string_view PopUpOkCancel = "/data/global/ui/FrontEnd/PopUpOKCancel.dc6";
} // namespace CharacterSelection

namespace Game {
inline constexpr std::string_view Panels = "/data/global/ui/PANEL/800ctrlpnl7.dc6";
inline constexpr std::string_view GlobeOverlap = "/data/global/ui/PANEL/overlap.DC6";
inline constexpr std::string_view HealthManaIndicator = "/data/global/ui/PANEL/hlthmana.DC6";
inline constexpr std::string_view AddSkillButton = "/data/global/ui/PANEL/level.DC6";
inline constexpr std::string_view MoveGoldDialog = "/data/global/ui/menu/dialogbackground.DC6";
inline constexpr std::string_view WPTabs = "/data/global/ui/menu/expwaygatetabs.dc6";
inline constexpr std::string_view WPBg = "/data/global/ui/menu/waygatebackground.dc6";
inline constexpr std::string_view WPIcons = "/data/global/ui/menu/waygateicons.dc6";
inline constexpr std::string_view UpDownArrows = "/data/global/ui/BIGMENU/numberarrows.dc6";
} // namespace Game

// --- Escape Menu ---
namespace EscapeMenu {
namespace Main {
inline constexpr std::string_view Options = "/data/local/ui/" OD2_LANGUAGE_TABLE_TOKEN "/options.dc6";
inline constexpr std::string_view Exit = "/data/local/ui/" OD2_LANGUAGE_TABLE_TOKEN "/exit.dc6";
inline constexpr std::string_view ReturnToGame = "/data/local/ui/" OD2_LANGUAGE_TABLE_TOKEN "/returntogame.dc6";
} // namespace Main
namespace Options {
inline constexpr std::string_view SoundOptions = "/data/local/ui/" OD2_LANGUAGE_TABLE_TOKEN "/soundoptions.dc6";
inline constexpr std::string_view VideoOptions = "/data/local/ui/" OD2_LANGUAGE_TABLE_TOKEN "/videoOptions.dc6";
inline constexpr std::string_view AutoMapOptions = "/data/local/ui/" OD2_LANGUAGE_TABLE_TOKEN "/automapOptions.dc6";
inline constexpr std::string_view CfgOptions = "/data/local/ui/" OD2_LANGUAGE_TABLE_TOKEN "/cfgOptions.dc6";
inline constexpr std::string_view Previous = "/data/local/ui/" OD2_LANGUAGE_TABLE_TOKEN "/previous.dc6";
} // namespace Options
} // namespace EscapeMenu

// sound options
inline constexpr std::string_view EscapeSndOptSoundVolume = "/data/local/ui/" OD2_LANGUAGE_TABLE_TOKEN "/sound.dc6";
inline constexpr std::string_view EscapeSndOptMusicVolume = "/data/local/ui/" OD2_LANGUAGE_TABLE_TOKEN "/music.dc6";
inline constexpr std::string_view EscapeSndOpt3DBias = "/data/local/ui/" OD2_LANGUAGE_TABLE_TOKEN "/3dbias.dc6";
inline constexpr std::string_view EscapeSndOptNPCSpeech = "/data/local/ui/" OD2_LANGUAGE_TABLE_TOKEN "/npcspeech.dc6";
inline constexpr std::string_view EscapeSndOptNPCSpeechAudioAndText = "/data/local/ui/" OD2_LANGUAGE_TABLE_TOKEN "/audiotext.dc6";
inline constexpr std::string_view EscapeSndOptNPCSpeechAudioOnly = "/data/local/ui/" OD2_LANGUAGE_TABLE_TOKEN "/audioonly.dc6";
inline constexpr std::string_view EscapeSndOptNPCSpeechTextOnly = "/data/local/ui/" OD2_LANGUAGE_TABLE_TOKEN "/textonly.dc6";

// video options
inline constexpr std::string_view EscapeVidOptRes = "/data/local/ui/" OD2_LANGUAGE_TABLE_TOKEN "/resolution.dc6";
inline constexpr std::string_view EscapeVidOptLightQuality = "/data/local/ui/" OD2_LANGUAGE_TABLE_TOKEN "/lightquality.dc6";
inline constexpr std::string_view EscapeVidOptBlendShadow = "/data/local/ui/" OD2_LANGUAGE_TABLE_TOKEN "/blendshadow.dc6";
inline constexpr std::string_view EscapeVidOptPerspective = "/data/local/ui/" OD2_LANGUAGE_TABLE_TOKEN "/prespective.dc6";
inline constexpr std::string_view EscapeVidOptGamma = "/data/local/ui/" OD2_LANGUAGE_TABLE_TOKEN "/gamma.dc6";
inline constexpr std::string_view EscapeVidOptContrast = "/data/local/ui/" OD2_LANGUAGE_TABLE_TOKEN "/contrast.dc6";

// auto map
inline constexpr std::string_view EscapeAutoMapOptSize = "/data/local/ui/" OD2_LANGUAGE_TABLE_TOKEN "/automapmode.dc6";
inline constexpr std::string_view EscapeAutoMapOptFade = "/data/local/ui/" OD2_LANGUAGE_TABLE_TOKEN "/automapfade.dc6";
inline constexpr std::string_view EscapeAutoMapOptCenter = "/data/local/ui/" OD2_LANGUAGE_TABLE_TOKEN "/automapcenter.dc6";
inline constexpr std::string_view EscapeAutoMapOptNames = "/data/local/ui/" OD2_LANGUAGE_TABLE_TOKEN "/automappartynames.dc6";

// automap size
inline constexpr std::string_view EscapeAutoMapOptFullScreen = "/data/local/ui/" OD2_LANGUAGE_TABLE_TOKEN "/full.dc6";
inline constexpr std::string_view EscapeAutoMapOptMiniMap = "/data/local/ui/" OD2_LANGUAGE_TABLE_TOKEN "/mini.dc6";

// resolutions
inline constexpr std::string_view EscapeVideoOptRes640x480 = "/data/local/ui/" OD2_LANGUAGE_TABLE_TOKEN "/640x480.dc6";
inline constexpr std::string_view EscapeVideoOptRes800x600 = "/data/local/ui/" OD2_LANGUAGE_TABLE_TOKEN "/800x800.dc6";

inline constexpr std::string_view EscapeOn = "/data/local/ui/" OD2_LANGUAGE_TABLE_TOKEN "/smallon.dc6";
inline constexpr std::string_view EscapeOff = "/data/local/ui/" OD2_LANGUAGE_TABLE_TOKEN "/smalloff.dc6";
inline constexpr std::string_view EscapeYes = "/data/local/ui/" OD2_LANGUAGE_TABLE_TOKEN "/smallyes.dc6";
inline constexpr std::string_view EscapeNo = "/data/local/ui/" OD2_LANGUAGE_TABLE_TOKEN "/smallno.dc6";
inline constexpr std::string_view EscapeSlideBar = "/data/global/ui/widgets/optbarc.dc6";
inline constexpr std::string_view EscapeSlideBarSkull = "/data/global/ui/widgets/optskull.dc6";

// --- Help Overlay ---

inline constexpr std::string_view HelpBorder = "/data/global/ui/MENU/800helpborder.DC6";
inline constexpr std::string_view HelpYellowBullet = "/data/global/ui/MENU/helpyellowbullet.DC6";
inline constexpr std::string_view HelpWhiteBullet = "/data/global/ui/MENU/helpwhitebullet.DC6";

// Box pieces, used in all in game boxes like npc interaction menu on click, the chat window and the key binding menu
inline constexpr std::string_view BoxPieces = "/data/global/ui/MENU/boxpieces.DC6";

// TextSlider contains the pieces to build a scrollbar in the menus, such as the one in the configure keys menu
inline constexpr std::string_view TextSlider = "/data/global/ui/MENU/textslid.DC6";

// Issue #685 - used in the mini-panel
inline constexpr std::string_view GameSmallMenuButton = "/data/global/ui/PANEL/menubutton.DC6";
inline constexpr std::string_view SkillIcon = "/data/global/ui/PANEL/Skillicon.DC6";

namespace QuestLog {
inline constexpr std::string_view QuestLogBg = "/data/global/ui/MENU/questbackground.dc6";
inline constexpr std::string_view QuestLogDone = "/data/global/ui/MENU/questdone.dc6";
inline constexpr std::string_view QuestLogTabs = "/data/global/ui/MENU/expquesttabs.dc6";
inline constexpr std::string_view QuestLogQDescrBtn = "/data/global/ui/MENU/questlast.dc6";
inline constexpr std::string_view QuestLogSocket = "/data/global/ui/MENU/questsockets.dc6";
inline constexpr std::string_view QuestLogAQuestAnimation = "/data/global/ui/MENU/a%dq%d.dc6";
inline constexpr std::string_view QuestLogDoneSfx = "cursor/questdone.wav";
} // namespace QuestLog

namespace MousePointers {
inline constexpr std::string_view CursorDefault = "/data/global/ui/CURSOR/ohand.DC6";
}

namespace FontsAndLocales {
inline constexpr std::string_view Font6 = "/data/local/FONT/" OD2_LANGUAGE_FONT_TOKEN "/font6";
inline constexpr std::string_view Font8 = "/data/local/FONT/" OD2_LANGUAGE_FONT_TOKEN "/font8";
inline constexpr std::string_view Font16 = "/data/local/FONT/" OD2_LANGUAGE_FONT_TOKEN "/font16";
inline constexpr std::string_view Font24 = "/data/local/FONT/" OD2_LANGUAGE_FONT_TOKEN "/font24";
inline constexpr std::string_view Font30 = "/data/local/FONT/" OD2_LANGUAGE_FONT_TOKEN "/font30";
inline constexpr std::string_view Font42 = "/data/local/FONT/" OD2_LANGUAGE_FONT_TOKEN "/font42";
inline constexpr std::string_view FontFormal12 = "/data/local/FONT/" OD2_LANGUAGE_FONT_TOKEN "/fontformal12";
inline constexpr std::string_view FontFormal11 = "/data/local/FONT/" OD2_LANGUAGE_FONT_TOKEN "/fontformal11";
inline constexpr std::string_view FontFormal10 = "/data/local/FONT/" OD2_LANGUAGE_FONT_TOKEN "/fontformal10";
inline constexpr std::string_view FontExocet10 = "/data/local/FONT/" OD2_LANGUAGE_FONT_TOKEN "/fontexocet10";
inline constexpr std::string_view FontExocet8 = "/data/local/FONT/" OD2_LANGUAGE_FONT_TOKEN "/fontexocet8";
inline constexpr std::string_view FontSucker = "/data/local/FONT/" OD2_LANGUAGE_FONT_TOKEN "/ReallyTheLastSucker";
inline constexpr std::string_view FontRediculous = "/data/local/FONT/" OD2_LANGUAGE_FONT_TOKEN "/fontridiculous";
inline constexpr std::string_view ExpansionStringTable = "/data/local/lng/" OD2_LANGUAGE_TABLE_TOKEN "/expansionstring.tbl";
inline constexpr std::string_view StringTable = "/data/local/lng/" OD2_LANGUAGE_TABLE_TOKEN "/string.tbl";
inline constexpr std::string_view PatchStringTable = "/data/local/lng/" OD2_LANGUAGE_TABLE_TOKEN "/patchstring.tbl";
} // namespace FontsAndLocales

namespace UI {
inline constexpr std::string_view WideButtonBlank = "/data/global/ui/FrontEnd/WideButtonBlank.dc6";
inline constexpr std::string_view MediumButtonBlank = "/data/global/ui/FrontEnd/MediumButtonBlank.dc6";
inline constexpr std::string_view CancelButton = "/data/global/ui/FrontEnd/CancelButtonBlank.dc6";
inline constexpr std::string_view NarrowButtonBlank = "/data/global/ui/FrontEnd/NarrowButtonBlank.dc6";
inline constexpr std::string_view ShortButtonBlank = "/data/global/ui/CharSelect/ShortButtonBlank.dc6";
inline constexpr std::string_view TextBox2 = "/data/global/ui/FrontEnd/textbox2.dc6";
inline constexpr std::string_view TallButtonBlank = "/data/global/ui/CharSelect/TallButtonBlank.dc6";
inline constexpr std::string_view Checkbox = "/data/global/ui/FrontEnd/clickbox.dc6";
inline constexpr std::string_view Scrollbar = "/data/global/ui/PANEL/scrollbar.dc6";

inline constexpr std::string_view PopUpLarge = "/data/global/ui/FrontEnd/PopUpLarge.dc6";
inline constexpr std::string_view PopUpLargest = "/data/global/ui/FrontEnd/PopUpLargest.dc6";
inline constexpr std::string_view PopUpWide = "/data/global/ui/FrontEnd/PopUpWide.dc6";
inline constexpr std::string_view PopUpOk = "/data/global/ui/FrontEnd/PopUpOk.dc6";
inline constexpr std::string_view PopUpOk2 = "/data/global/ui/FrontEnd/PopUpOk.dc6";
inline constexpr std::string_view PopUpOkCancel2 = "/data/global/ui/FrontEnd/PopUpOkCancel2.dc6";
inline constexpr std::string_view PopUp340x224 = "/data/global/ui/FrontEnd/PopUp_340x224.dc6";
} // namespace UI

namespace GameUI {
inline constexpr std::string_view PentSpin = "/data/global/ui/CURSOR/pentspin.DC6";
inline constexpr std::string_view Minipanel = "/data/global/ui/PANEL/minipanel.DC6";
inline constexpr std::string_view MinipanelSmall = "/data/global/ui/PANEL/minipanel_s.dc6";
inline constexpr std::string_view MinipanelButton = "/data/global/ui/PANEL/minipanelbtn.DC6";

inline constexpr std::string_view Frame = "/data/global/ui/PANEL/800borderframe.dc6";
inline constexpr std::string_view InventoryCharacterPanel = "/data/global/ui/PANEL/invchar6.DC6";
inline constexpr std::string_view PartyPanel = "/data/global/ui/MENU/party.dc6";
inline constexpr std::string_view PartyButton = "/data/global/ui/MENU/partybuttons.dc6";
inline constexpr std::string_view PartyBoxes = "/data/global/ui/MENU/partyboxes.dc6";
inline constexpr std::string_view PartyBar = "/data/global/ui/MENU/partybar.dc6";
inline constexpr std::string_view HeroStatsPanelStatsPoints = "/data/global/ui/PANEL/skillpoints.dc6";
inline constexpr std::string_view HeroStatsPanelSocket = "/data/global/ui/PANEL/levelsocket.dc6";
inline constexpr std::string_view InventoryWeaponsTab = "/data/global/ui/PANEL/invchar6Tab.DC6";
inline constexpr std::string_view SkillsPanelAmazon = "/data/global/ui/SPELLS/skltree_a_back.DC6";
inline constexpr std::string_view SkillsPanelBarbarian = "/data/global/ui/SPELLS/skltree_b_back.DC6";
inline constexpr std::string_view SkillsPanelDruid = "/data/global/ui/SPELLS/skltree_d_back.DC6";
inline constexpr std::string_view SkillsPanelAssassin = "/data/global/ui/SPELLS/skltree_i_back.DC6";
inline constexpr std::string_view SkillsPanelNecromancer = "/data/global/ui/SPELLS/skltree_n_back.DC6";
inline constexpr std::string_view SkillsPanelPaladin = "/data/global/ui/SPELLS/skltree_p_back.DC6";
inline constexpr std::string_view SkillsPanelSorcerer = "/data/global/ui/SPELLS/skltree_s_back.DC6";

inline constexpr std::string_view GenericSkills = "/data/global/ui/SPELLS/Skillicon.DC6";
inline constexpr std::string_view AmazonSkills = "/data/global/ui/SPELLS/AmSkillicon.DC6";
inline constexpr std::string_view BarbarianSkills = "/data/global/ui/SPELLS/BaSkillicon.DC6";
inline constexpr std::string_view DruidSkills = "/data/global/ui/SPELLS/DrSkillicon.DC6";
inline constexpr std::string_view AssassinSkills = "/data/global/ui/SPELLS/AsSkillicon.DC6";
inline constexpr std::string_view NecromancerSkills = "/data/global/ui/SPELLS/NeSkillicon.DC6";
inline constexpr std::string_view PaladinSkills = "/data/global/ui/SPELLS/PaSkillicon.DC6";
inline constexpr std::string_view SorcererSkills = "/data/global/ui/SPELLS/SoSkillicon.DC6";

inline constexpr std::string_view RunButton = "/data/global/ui/PANEL/runbutton.dc6";
inline constexpr std::string_view MenuButton = "/data/global/ui/PANEL/menubutton.DC6";
inline constexpr std::string_view GoldCoinButton = "/data/global/ui/panel/goldcoinbtn.dc6";
inline constexpr std::string_view BuySellButton = "/data/global/ui/panel/buysellbtn.dc6";

inline constexpr std::string_view ArmorPlaceholder = "/data/global/ui/PANEL/inv_armor.DC6";
inline constexpr std::string_view BeltPlaceholder = "/data/global/ui/PANEL/inv_belt.DC6";
inline constexpr std::string_view BootsPlaceholder = "/data/global/ui/PANEL/inv_boots.DC6";
inline constexpr std::string_view HelmGlovePlaceholder = "/data/global/ui/PANEL/inv_helm_glove.DC6";
inline constexpr std::string_view RingAmuletPlaceholder = "/data/global/ui/PANEL/inv_ring_amulet.DC6";
inline constexpr std::string_view WeaponsPlaceholder = "/data/global/ui/PANEL/inv_weapons.DC6";
} // namespace GameUI

namespace Data {
inline constexpr std::string_view LevelPreset = "/data/global/excel/LvlPrest.txt";
inline constexpr std::string_view LevelType = "/data/global/excel/LvlTypes.txt";
inline constexpr std::string_view ObjectType = "/data/global/excel/objtype.txt";
inline constexpr std::string_view LevelWarp = "/data/global/excel/LvlWarp.txt";
inline constexpr std::string_view LevelDetails = "/data/global/excel/Levels.txt";
inline constexpr std::string_view LevelMaze = "/data/global/excel/LvlMaze.txt";
inline constexpr std::string_view LevelSubstitutions = "/data/global/excel/LvlSub.txt";

inline constexpr std::string_view ObjectDetails = "/data/global/excel/Objects.txt";
inline constexpr std::string_view ObjectMode = "/data/global/excel/ObjMode.txt";
inline constexpr std::string_view SoundSettings = "/data/global/excel/Sounds.txt";
inline constexpr std::string_view ItemStatCost = "/data/global/excel/ItemStatCost.txt";
inline constexpr std::string_view ItemRatio = "/data/global/excel/itemratio.txt";
inline constexpr std::string_view ItemTypes = "/data/global/excel/ItemTypes.txt";
inline constexpr std::string_view QualityItems = "/data/global/excel/qualityitems.txt";
inline constexpr std::string_view LowQualityItems = "/data/global/excel/lowqualityitems.txt";
inline constexpr std::string_view Overlays = "/data/global/excel/Overlay.txt";
inline constexpr std::string_view Runes = "/data/global/excel/runes.txt";
inline constexpr std::string_view Sets = "/data/global/excel/Sets.txt";
inline constexpr std::string_view SetItems = "/data/global/excel/SetItems.txt";
inline constexpr std::string_view AutoMagic = "/data/global/excel/automagic.txt";
inline constexpr std::string_view BodyLocations = "/data/global/excel/bodylocs.txt";
inline constexpr std::string_view Events = "/data/global/excel/events.txt";
inline constexpr std::string_view Properties = "/data/global/excel/Properties.txt";
inline constexpr std::string_view Hireling = "/data/global/excel/hireling.txt";
inline constexpr std::string_view HirelingDescription = "/data/global/excel/HireDesc.txt";
inline constexpr std::string_view DifficultyLevels = "/data/global/excel/difficultylevels.txt";
inline constexpr std::string_view AutoMap = "/data/global/excel/AutoMap.txt";
inline constexpr std::string_view CubeRecipes = "/data/global/excel/cubemain.txt";
inline constexpr std::string_view CubeModifier = "/data/global/excel/CubeMod.txt";
inline constexpr std::string_view CubeType = "/data/global/excel/CubeType.txt";
inline constexpr std::string_view Skills = "/data/global/excel/skills.txt";
inline constexpr std::string_view SkillDesc = "/data/global/excel/skilldesc.txt";
inline constexpr std::string_view SkillCalc = "/data/global/excel/skillcalc.txt";
inline constexpr std::string_view MissileCalc = "/data/global/excel/misscalc.txt";
inline constexpr std::string_view TreasureClass = "/data/global/excel/TreasureClass.txt";
inline constexpr std::string_view TreasureClassEx = "/data/global/excel/TreasureClassEx.txt";
inline constexpr std::string_view States = "/data/global/excel/states.txt";
inline constexpr std::string_view SoundEnvirons = "/data/global/excel/soundenviron.txt";
inline constexpr std::string_view Shrines = "/data/global/excel/shrines.txt";
inline constexpr std::string_view MonProp = "/data/global/excel/Monprop.txt";
inline constexpr std::string_view ElemType = "/data/global/excel/ElemTypes.txt";
inline constexpr std::string_view PlrMode = "/data/global/excel/PlrMode.txt";
inline constexpr std::string_view PetType = "/data/global/excel/pettype.txt";
inline constexpr std::string_view NPC = "/data/global/excel/npc.txt";
inline constexpr std::string_view MonsterUniqueModifier = "/data/global/excel/monumod.txt";
inline constexpr std::string_view MonsterEquipment = "/data/global/excel/monequip.txt";
inline constexpr std::string_view UniqueAppellation = "/data/global/excel/UniqueAppellation.txt";
inline constexpr std::string_view MonsterLevel = "/data/global/excel/monlvl.txt";
inline constexpr std::string_view MonsterSound = "/data/global/excel/monsounds.txt";
inline constexpr std::string_view MonsterSequence = "/data/global/excel/monseq.txt";
inline constexpr std::string_view PlayerClass = "/data/global/excel/PlayerClass.txt";
inline constexpr std::string_view PlayerType = "/data/global/excel/PlrType.txt";
inline constexpr std::string_view Composite = "/data/global/excel/Composit.txt";
inline constexpr std::string_view HitClass = "/data/global/excel/HitClass.txt";
inline constexpr std::string_view ObjectGroup = "/data/global/excel/objgroup.txt";
inline constexpr std::string_view CompCode = "/data/global/excel/compcode.txt";
inline constexpr std::string_view Belts = "/data/global/excel/belts.txt";
inline constexpr std::string_view Gamble = "/data/global/excel/gamble.txt";
inline constexpr std::string_view Colors = "/data/global/excel/colors.txt";
inline constexpr std::string_view StorePage = "/data/global/excel/StorePage.txt";
} // namespace Data

namespace Animations {
inline constexpr std::string_view ObjectData = "/data/global/objects";
inline constexpr std::string_view AnimationData = "/data/global/animdata.d2";
inline constexpr std::string_view PlayerAnimationBase = "/data/global/CHARS";
inline constexpr std::string_view MissileData = "/data/global/missiles";
inline constexpr std::string_view ItemGraphics = "/data/global/items";
} // namespace Animations

namespace InventoryData {
inline constexpr std::string_view Inventory = "/data/global/excel/inventory.txt";
inline constexpr std::string_view Weapons = "/data/global/excel/weapons.txt";
inline constexpr std::string_view Armor = "/data/global/excel/armor.txt";
inline constexpr std::string_view ArmorType = "/data/global/excel/ArmType.txt";
inline constexpr std::string_view WeaponClass = "/data/global/excel/WeaponClass.txt";
inline constexpr std::string_view Books = "/data/global/excel/books.txt";
inline constexpr std::string_view Misc = "/data/global/excel/misc.txt";
inline constexpr std::string_view UniqueItems = "/data/global/excel/UniqueItems.txt";
inline constexpr std::string_view Gems = "/data/global/excel/gems.txt";
} // namespace InventoryData

namespace Affixes {
inline constexpr std::string_view MagicPrefix = "/data/global/excel/MagicPrefix.txt";
inline constexpr std::string_view MagicSuffix = "/data/global/excel/MagicSuffix.txt";
inline constexpr std::string_view RarePrefix = "/data/global/excel/RarePrefix.txt"; // these are for item names
inline constexpr std::string_view RareSuffix = "/data/global/excel/RareSuffix.txt";
} // namespace Affixes

namespace MonsterPrefixSuffixes {
inline constexpr std::string_view UniquePrefix = "/data/global/excel/UniquePrefix.txt";
inline constexpr std::string_view UniqueSuffix = "/data/global/excel/UniqueSuffix.txt";
} // namespace MonsterPrefixSuffixes

namespace CharacterData {
inline constexpr std::string_view Experience = "/data/global/excel/experience.txt";
inline constexpr std::string_view CharStats = "/data/global/excel/charstats.txt";
} // namespace CharacterData

namespace Music {
inline constexpr std::string_view Title = "/data/global/music/introedit.wav";
inline constexpr std::string_view Options = "/data/global/music/Common/options.wav";
inline constexpr std::string_view Act1AndarielAction = "/data/global/music/Act1/andarielaction.wav";
inline constexpr std::string_view Act1BloodRavenResolution = "/data/global/music/Act1/bloodravenresolution.wav";
inline constexpr std::string_view Act1Caves = "/data/global/music/Act1/caves.wav";
inline constexpr std::string_view Act1Crypt = "/data/global/music/Act1/crypt.wav";
inline constexpr std::string_view Act1DenOfEvilAction = "/data/global/music/Act1/denofevilaction.wav";
inline constexpr std::string_view Act1Monastery = "/data/global/music/Act1/monastery.wav";
inline constexpr std::string_view Act1Town1 = "/data/global/music/Act1/town1.wav";
inline constexpr std::string_view Act1Tristram = "/data/global/music/Act1/tristram.wav";
inline constexpr std::string_view Act1Wild = "/data/global/music/Act1/wild.wav";
inline constexpr std::string_view Act2Desert = "/data/global/music/Act2/desert.wav";
inline constexpr std::string_view Act2Harem = "/data/global/music/Act2/harem.wav";
inline constexpr std::string_view Act2HoradricAction = "/data/global/music/Act2/horadricaction.wav";
inline constexpr std::string_view Act2Lair = "/data/global/music/Act2/lair.wav";
inline constexpr std::string_view Act2RadamentResolution = "/data/global/music/Act2/radamentresolution.wav";
inline constexpr std::string_view Act2Sanctuary = "/data/global/music/Act2/sanctuary.wav";
inline constexpr std::string_view Act2Sewer = "/data/global/music/Act2/sewer.wav";
inline constexpr std::string_view Act2TaintedSunAction = "/data/global/music/Act2/taintedsunaction.wav";
inline constexpr std::string_view Act2Tombs = "/data/global/music/Act2/tombs.wav";
inline constexpr std::string_view Act2Town2 = "/data/global/music/Act2/town2.wav";
inline constexpr std::string_view Act2Valley = "/data/global/music/Act2/valley.wav";
inline constexpr std::string_view Act3Jungle = "/data/global/music/Act3/jungle.wav";
inline constexpr std::string_view Act3Kurast = "/data/global/music/Act3/kurast.wav";
inline constexpr std::string_view Act3KurastSewer = "/data/global/music/Act3/kurastsewer.wav";
inline constexpr std::string_view Act3MefDeathAction = "/data/global/music/Act3/mefdeathaction.wav";
inline constexpr std::string_view Act3OrbAction = "/data/global/music/Act3/orbaction.wav";
inline constexpr std::string_view Act3Spider = "/data/global/music/Act3/spider.wav";
inline constexpr std::string_view Act3Town3 = "/data/global/music/Act3/town3.wav";
inline constexpr std::string_view Act4Diablo = "/data/global/music/Act4/diablo.wav";
inline constexpr std::string_view Act4DiabloAction = "/data/global/music/Act4/diabloaction.wav";
inline constexpr std::string_view Act4ForgeAction = "/data/global/music/Act4/forgeaction.wav";
inline constexpr std::string_view Act4IzualAction = "/data/global/music/Act4/izualaction.wav";
inline constexpr std::string_view Act4Mesa = "/data/global/music/Act4/mesa.wav";
inline constexpr std::string_view Act4Town4 = "/data/global/music/Act4/town4.wav";
inline constexpr std::string_view Act5Baal = "/data/global/music/Act5/baal.wav";
inline constexpr std::string_view Act5Siege = "/data/global/music/Act5/siege.wav";
inline constexpr std::string_view Act5Shenk = "/data/global/music/Act5/shenkmusic.wav";
inline constexpr std::string_view Act5XTown = "/data/global/music/Act5/xtown.wav";
inline constexpr std::string_view Act5XTemple = "/data/global/music/Act5/xtemple.wav";
inline constexpr std::string_view Act5IceCaves = "/data/global/music/Act5/icecaves.wav";
inline constexpr std::string_view Act5Nihlathak = "/data/global/music/Act5/nihlathakmusic.wav";
} // namespace Music

namespace SFX {
inline constexpr std::string_view CursorSelect = "cursor_select";
inline constexpr std::string_view ButtonClick = "/data/global/sfx/cursor/button.wav";
inline constexpr std::string_view AmazonDeselect = "cursor_amazon_deselect";
inline constexpr std::string_view AmazonSelect = "cursor_amazon_select";
inline constexpr std::string_view AssassinDeselect = "Cursor/intro/assassin deselect.wav";
inline constexpr std::string_view AssassinSelect = "Cursor/intro/assassin select.wav";
inline constexpr std::string_view BarbarianDeselect = "cursor_barbarian_deselect";
inline constexpr std::string_view BarbarianSelect = "cursor_barbarian_select";
inline constexpr std::string_view DruidDeselect = "Cursor/intro/druid deselect.wav";
inline constexpr std::string_view DruidSelect = "Cursor/intro/druid select.wav";
inline constexpr std::string_view NecromancerDeselect = "cursor_necromancer_deselect";
inline constexpr std::string_view NecromancerSelect = "cursor_necromancer_select";
inline constexpr std::string_view PaladinDeselect = "cursor_paladin_deselect";
inline constexpr std::string_view PaladinSelect = "cursor_paladin_select";
inline constexpr std::string_view SorceressDeselect = "cursor_sorceress_deselect";
inline constexpr std::string_view SorceressSelect = "cursor_sorceress_select";
} // namespace SFX

// --- Enemy Data ---
namespace EnemyData {
inline constexpr std::string_view MonStats = "/data/global/excel/monstats.txt";
inline constexpr std::string_view MonStats2 = "/data/global/excel/monstats2.txt";
inline constexpr std::string_view MonPreset = "/data/global/excel/monpreset.txt";
inline constexpr std::string_view MonType = "/data/global/excel/Montype.txt";
inline constexpr std::string_view SuperUniques = "/data/global/excel/SuperUniques.txt";
inline constexpr std::string_view MonMode = "/data/global/excel/monmode.txt";
inline constexpr std::string_view MonsterPlacement = "/data/global/excel/MonPlace.txt";
inline constexpr std::string_view MonsterAI = "/data/global/excel/monai.txt";
} // namespace EnemyData

namespace SkillData {
inline constexpr std::string_view Missiles = "/data/global/excel/Missiles.txt";
}

namespace Palettes {
inline constexpr std::string_view Act1 = "/data/global/palette/act1/pal.dat";
inline constexpr std::string_view Act2 = "/data/global/palette/act2/pal.dat";
inline constexpr std::string_view Act3 = "/data/global/palette/act3/pal.dat";
inline constexpr std::string_view Act4 = "/data/global/palette/act4/pal.dat";
inline constexpr std::string_view Act5 = "/data/global/palette/act5/pal.dat";
inline constexpr std::string_view EndGame = "/data/global/palette/endgame/pal.dat";
inline constexpr std::string_view EndGame2 = "/data/global/palette/endgame2/pal.dat";
inline constexpr std::string_view Fechar = "/data/global/palette/fechar/pal.dat";
inline constexpr std::string_view Loading = "/data/global/palette/loading/pal.dat";
inline constexpr std::string_view Menu0 = "/data/global/palette/menu0/pal.dat";
inline constexpr std::string_view Menu1 = "/data/global/palette/menu1/pal.dat";
inline constexpr std::string_view Menu2 = "/data/global/palette/menu2/pal.dat";
inline constexpr std::string_view Menu3 = "/data/global/palette/menu3/pal.dat";
inline constexpr std::string_view Menu4 = "/data/global/palette/menu4/pal.dat";
inline constexpr std::string_view Sky = "/data/global/palette/sky/pal.dat";
inline constexpr std::string_view Static = "/data/global/palette/static/pal.dat";
inline constexpr std::string_view Trademark = "/data/global/palette/trademark/pal.dat";
inline constexpr std::string_view Units = "/data/global/palette/units/pal.dat";
} // namespace Palettes

namespace PaletteTransforms {
inline constexpr std::string_view Act1 = "/data/global/palette/act1/Pal.pl2";
inline constexpr std::string_view Act2 = "/data/global/palette/act2/Pal.pl2";
inline constexpr std::string_view Act3 = "/data/global/palette/act3/Pal.pl2";
inline constexpr std::string_view Act4 = "/data/global/palette/act4/Pal.pl2";
inline constexpr std::string_view Act5 = "/data/global/palette/act5/Pal.pl2";
inline constexpr std::string_view EndGame = "/data/global/palette/endgame/Pal.pl2";
inline constexpr std::string_view EndGame2 = "/data/global/palette/endgame2/Pal.pl2";
inline constexpr std::string_view Fechar = "/data/global/palette/fechar/Pal.pl2";
inline constexpr std::string_view Loading = "/data/global/palette/loading/Pal.pl2";
inline constexpr std::string_view Menu0 = "/data/global/palette/menu0/Pal.pl2";
inline constexpr std::string_view Menu1 = "/data/global/palette/menu1/Pal.pl2";
inline constexpr std::string_view Menu2 = "/data/global/palette/menu2/Pal.pl2";
inline constexpr std::string_view Menu3 = "/data/global/palette/menu3/Pal.pl2";
inline constexpr std::string_view Menu4 = "/data/global/palette/menu4/Pal.pl2";
inline constexpr std::string_view Sky = "/data/global/palette/sky/Pal.pl2";
inline constexpr std::string_view Trademark = "/data/global/palette/trademark/Pal.pl2";
} // namespace PaletteTransforms
} // namespace OD2::Common::ResourcePaths

#undef OD2_LANGUAGE_FONT_TOKEN
#undef OD2_LANGUAGE_TABLE_TOKEN
