#ifndef INTERFACES
#define INTERFACES

#include <string>
#include "Login.h"

using namespace std; 


class Interfaces{
	
	Login login;

	Logger log;

	ObjectBuilder builder;



	void _findByText(PyObject * parentInt, string text, PyObject ** obj);
	PyObject * _getAbsoluteLeft(PyObject * result);
	PyObject * _getAbsoluteTop(PyObject * result);
	PyObject * _getName(PyObject * result);
	PyObject * _getAttribute(PyObject * result, string attr);
	PyObject * _getDisplayWidth(PyObject * result);
	PyObject * _getDisplayHeight(PyObject * result);
	PyObject * Interfaces::_getText(PyObject * result);
	PyObject * _getHeight(PyObject * result);
	PyObject * _getWidth(PyObject * result);
	PyObject * _findByNameLayer(PyObject * layer, string name);
	PyObject * _getLayer(string layername);
	PyObject * _findModule(string module);
	PyObject * _GetEntry(string name);
	PyObject * _GetInflightCargoView();
	PyObject * _findType(string name, PyObject * children);
	PyObject * _getNeocomButton(string buttonname);
	bool _populateAttributes(PyObject * item, PyObject ** width, PyObject ** height, PyObject ** absoluteTop, PyObject ** absoluteLeft);
	bool _populateAttributesDisplay(PyObject * item, PyObject ** width, PyObject ** height, PyObject ** absoluteTop, PyObject ** absoluteLeft);
	void _IterateThroughEntryAndBuild(PyObject * entry, list<ObjectBuilder::itemEntry *> & labels );

	char * _findByTextGeneric(string layername, string label, int & size);
	char * _findByNameGeneric(string layername, string name, int & size);
	char * _getLayerWithAttributes(string layername, int & size);
	char * _buildModule(PyObject * mod, string name, int & size);
	char * _GetSlot(string name, string outputname, int & size);
	char * _isModuleActive(string name, int & size);
	char * _GetShipUIGauge(string name, int & size);	
	char * _getModalButton(string name, int & size);
	char * _getModuleTargetingRange(string name, int & size);
	char * _getDroneLabel(int type, int & size);
	char * _getDroneStatus(int & size);
	char * _getMiningAmount(string name, int & size);
	char * _getModuleDuration(string name, int & size);
	char * _getModuleAttribute(string name, string attr, int & size);
	PyObject * _getScrollHandle(PyObject * layer);
	int _getSize(PyObject * layer);
	PyObject * _getOverviewScroll();
	PyObject * _getSysMenuButtonByText(string text);
	PyObject * _getLocalChatScroll();
	char * _getLocalChatScrollAttribute(string attr, int & size);
	PyObject * _getLoginItem(string name);
	char * _getLoginBoxesWithText(string name, int & size);
	char * _getNeoComItem(string name, int & size);
	PyObject * _getAddressBookWindow();
	char * _getPeopleAndPlacesButton(string name, int & size);
	char * _getOverViewHeaders(string name, int & size);
	PyObject * _getStationLobbyBottom();
	char * _getLobbyTab(string name, int & size);
	void _strToLower(string & str);
	PyObject * _getAgentWindow();
	char * _getAgentButton(string name, int & size);
	char * _getProbeButton(string name, int & size);
	char * _findColumnEntryProbeWindow(string name, PyObject * layer, int & size);
	char * _getModuleInfo(string name, int & size);
	char * _getModuleAttributeCA(string name, string attr, int & size); ///This allocates mem
	char * _getModuleAttributes(string name, int & size);
	char * _getMarketOrders(string type, int & size);
	void clearExceptions();

public:
	char * getInjuredDrone(int & size);
	char * GetMarketSearchEditContent(int & size);
	char * GetMarketSearchEdit(int & size);
	char * GetMarketSearchButton(int & size);
	char * GetMarketSearchResult(int & size);
	char * GetBuyOrders(int & size);
	char * GetSellOrders(int & size);
	char * GetHighSlotAttributes(int number, int & size);
	char * GetHighSlotModuleInfo(int number, int & size);
	char * GetMedSlotModuleInfo(int number, int & size);
	char * GetLowSlotModuleInfo(int number, int & size);
	char * GetProbeResult(string name, int & size);
	char * GetProbe(string name, int & size);
	char * GetAnalyzeProbesButton(int & size);
	char * GetRecoverProbesButton(int & size);
	char * GetAgentMissionCloseBtn(int & size);
	char * GetAgentMissionText(int & size);
	char * GetAgentMissionQuitBtn(int & size);
	char * GetAgentMissionCompleteBtn(int & size);
	char * GetAgentMissionDelayBtn(int & size);
	char * GetAgentMissionDeclineBtn(int & size);
	char * GetAgentMissionAcceptBtn(int & size);
	char * GetAgentReqMissionBtn(int & size);
	char * GetAgent(string name, int & size);
	char * GetStationAgentTab(int & size);
	char * GetOverviewDistanceHeader(int & size);
	char * GetShipCapacitor(int & size);
	char * GetLocalWritingArea(int & size);
	char * GetCurrentSolarsystemid(int & size);
	char * GetLocalChatText(int sysid, int & size);
	char * GetOverViewSelectIcon(int & size);
	char * GetOverviewSelectText(int & size);
	char * GetNeoComItems(int & size);
	char * GetLocalCount(int & size);
	char * GetAddressBookWindow(int & size);
	char * GetAddressBookBMButton(int & size);
	char * GetAddressBookPlacesTab(int & size);
	char * GetPeopleAndPlaces(int & size);
	char * GetHangarItems(int & size);
	char * getInventoryWindow(int & size);
	char * getHangar(int & size);
	char * IsMedSlotActive(int number, int & size);
	char * IsLowSlotActive(int number, int & size);
	char * GetLowSlot(int number, int & size);
	char * getEnterButton(int & size);
	char * isAtCharSel(int & size);
	char * getConnectButton(int & size);
	char * getUserNameBox(int & size);
	char * getPasswordBox(int & size);
	char * IsFleeted(int & size);
	string Internal_getVersion();
	char * GetLocalChatScrollbar(int & size);
	char * GetLocalChatTop(int & size);
	char * GetLocalChatBottom(int & size);
	char * FindPlayerInLocal(string name, int & size);
	char * GetModalYesButton(int & size);
	char * GetModalNoButton(int & size);
	char * GetLogOffButton(int & size);
	char * GetBookMarkFieldName(int & size);
	bool isLoginOpen();
	char * GetVersion(int & size);
	char * GetOverviewTop(int & size);
	char * GetOverviewBottom(int & size);
	char * GetOverviewHeight(int & size);
	char * OverviewGetScrollBar(int & size);
	char * GetMiningAmount(int number, int & size);
	char * GetDuration(int type, int & size);
	char * CheckDroneStatus(int & size);
	char * CheckLocal(int & size);
	char * DronesInFlight(int & size);
	char * DronesInBay(int & size);
	char * IsIncursion(int & size);
	char * GetInterfaceWindows(int & size);
	char * GetTargetingRange(int number, int & size);
	char * IsLoading(int & size);
	char * GetServerMessage(int & size);
	char * GetSystemInformation(int & size);
	char * GetModalCancelButton(int & size);
	char * GetModalOkButton(int & size);
	char * IsSystemMenuOpen(int & size);
	char * GetMenuItems(int & size);
	char * GetShipCapacity(int & size);
	char * GetShipArmor(int & size);
	char * GetShipShield(int & size);
	char * GetShipStructure(int & size);
	char * GetShipSpeed(int & size);
	char * GetStationHangar(int & size);
	char * GetStationItemsButton(int & size);
	char * GetCargoList(int & size);
	char * GetUndockButton(int & size);
	char * IsHighSlotActive(int number, int & size);
	char * GetHighSlot(int number, int & size);
	char * GetMidSlot(int number, int & size);
	char * GetTargetList(int & size);
	char * GetSelectedItem(int & size);
	char * isMenuOpen(int & size);
	char * OverViewGetMembers(int & size);
	char * getInflightInterface(int & size);
	char * atLogin(int & size);
	char * findByTextMenu(string label, int & size);
	char * findByTextMenuContains(string label, int & size);
	char * findByNameLogin(string name, int & size);
	char * findByTextLogin(string text, int & size);
	char * GetShipHangar(int & size);
	char * _getInjuredDrone(int & size);
};

#endif