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
public:
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
};

#endif