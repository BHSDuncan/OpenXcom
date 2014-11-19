/*
 * Copyright 2010-2014 OpenXcom Developers.
 *
 * This file is part of OpenXcom.
 *
 * OpenXcom is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenXcom is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenXcom.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "SoldiersState.h"
#include "../Engine/Action.h"
#include <string>
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Engine/Options.h"
#include "../Geoscape/AllocatePsiTrainingState.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Savegame/Base.h"
#include "../Savegame/Soldier.h"
#include "../Savegame/Craft.h"
#include "../Ruleset/RuleCraft.h"
#include "SoldierInfoState.h"
#include "SoldierMemorialState.h"

namespace OpenXcom
{

	/**
	 * Initializes all the elements in the Soldiers screen.
	 * @param game Pointer to the core game.
	 * @param base Pointer to the base to get info from.
	 */
	SoldiersState::SoldiersState(Base *base) : _base(base), _curCraftInList(-2)
	{
		bool isPsiBtnVisible = Options::anytimePsiTraining && _base->getAvailablePsiLabs() > 0;

		// Create objects
		_window = new Window(this, 320, 200, 0, 0);
		if (isPsiBtnVisible)
		{
			_btnOk = new TextButton(96, 16, 216, 176);
			_btnPsiTraining = new TextButton(96, 16, 112, 176);
			_btnMemorial = new TextButton(96, 16, 8, 176);
		}
		else
		{
			_btnOk = new TextButton(148, 16, 164, 176);
			_btnPsiTraining = new TextButton(148, 16, 164, 176);
			_btnMemorial = new TextButton(148, 16, 8, 176);
		}
		_txtTitle = new Text(310, 17, 5, 8);
		_txtName = new Text(114, 9, 16, 32);
		_txtRank = new Text(102, 9, 130, 32);
		_txtCraft = new Text(82, 9, 222, 32);
		_lstSoldiers = new TextList(288, 128, 8, 40);

		// Set palette
		setPalette("PAL_BASESCAPE", 2);

		add(_window);
		add(_btnOk);
		add(_btnPsiTraining);
		add(_btnMemorial);
		add(_txtTitle);
		add(_txtName);
		add(_txtRank);
		add(_txtCraft);
		add(_lstSoldiers);

		centerAllSurfaces();

		// Set up objects
		_window->setColor(Palette::blockOffset(15) + 1);
		_window->setBackground(_game->getResourcePack()->getSurface("BACK02.SCR"));

		_btnOk->setColor(Palette::blockOffset(13) + 10);
		_btnOk->setText(tr("STR_OK"));
		_btnOk->onMouseClick((ActionHandler)&SoldiersState::btnOkClick);
		_btnOk->onKeyboardPress((ActionHandler)&SoldiersState::btnOkClick, Options::keyCancel);

		_btnPsiTraining->setColor(Palette::blockOffset(13) + 10);
		_btnPsiTraining->setText(tr("STR_PSIONIC_TRAINING"));
		_btnPsiTraining->onMouseClick((ActionHandler)&SoldiersState::btnPsiTrainingClick);
		_btnPsiTraining->setVisible(isPsiBtnVisible);

		_btnMemorial->setColor(Palette::blockOffset(13) + 10);
		_btnMemorial->setText(tr("STR_MEMORIAL"));
		_btnMemorial->onMouseClick((ActionHandler)&SoldiersState::btnMemorialClick);

		_txtTitle->setColor(Palette::blockOffset(13) + 10);
		_txtTitle->setBig();
		_txtTitle->setAlign(ALIGN_CENTER);
		_txtTitle->setText(tr("STR_SOLDIER_LIST"));

		_txtName->setColor(Palette::blockOffset(15) + 1);
		_txtName->setText(tr("STR_NAME_UC"));

		_txtRank->setColor(Palette::blockOffset(15) + 1);
		_txtRank->setText(tr("STR_RANK"));

		_txtCraft->setColor(Palette::blockOffset(15) + 1);
		_txtCraft->setText(tr("STR_CRAFT"));

		_lstSoldiers->setColor(Palette::blockOffset(13) + 10);
		_lstSoldiers->setArrowColor(Palette::blockOffset(15) + 1);
		_lstSoldiers->setColumns(3, 114, 92, 74);
		_lstSoldiers->setMultiSelectable(true);
		_lstSoldiers->setBackground(_window);
		_lstSoldiers->setMargin(8);
		_lstSoldiers->onMouseClick((ActionHandler)&SoldiersState::lstSoldiersClick);
	}

	/**
	 *
	 */
	SoldiersState::~SoldiersState()
	{

	}

	/**
	 * Updates the soldiers list
	 * after going to other screens.
	 */
	void SoldiersState::init()
	{
		State::init();
		unsigned int row = 0;
		_lstSoldiers->clearList();
		for (std::vector<Soldier*>::iterator i = _base->getSoldiers()->begin(); i != _base->getSoldiers()->end(); ++i)
		{
			_lstSoldiers->addRow(3, (*i)->getName(true).c_str(), tr((*i)->getRankString()).c_str(), (*i)->getCraftString(_game->getLanguage()).c_str());
			if ((*i)->getCraft() == 0)
			{
				_lstSoldiers->setRowColor(row, Palette::blockOffset(15) + 6);
			}
			row++;
		}
		if (row > 0 && _lstSoldiers->getScroll() >= row)
		{
			_lstSoldiers->scrollTo(0);
		}
	}

	/**
	 * Returns to the previous screen.
	 * @param action Pointer to an action.
	 */
	void SoldiersState::btnOkClick(Action *)
	{
		_game->popState();
	}

	/**
	 * Opens the Psionic Training screen.
	 * @param action Pointer to an action.
	 */
	void SoldiersState::btnPsiTrainingClick(Action *)
	{
		_game->pushState(new AllocatePsiTrainingState(_base));
	}

	/**
	 * Opens the Memorial screen.
	 * @param action Pointer to an action.
	 */
	void SoldiersState::btnMemorialClick(Action *)
	{
		_game->pushState(new SoldierMemorialState);
	}

	/**
	 * Shows the selected soldier's info.
	 * @param action Pointer to an action.
	 */
	void SoldiersState::lstSoldiersClick(Action *action)
	{
		if (action->getDetails()->button.button == SDL_BUTTON_LEFT)
		{
			// only edit a soldier's info if there are currently no soldiers in "multi-select"
			if (_lstSoldiers->getNumSelectedRows() == 0)
			{
				_game->pushState(new SoldierInfoState(_base, _lstSoldiers->getSelectedRow()));
			}
			else
			{
				changeCraft();
			}
		}
		else if (action->getDetails()->button.button == SDL_BUTTON_RIGHT)
		{
			// reset the group craft list index
			_curCraftInList = -2;
		}
	}

	/**
	  * Updates the selected soldiers' assigned craft by cycling through the available ships with each call.
	  */
	void SoldiersState::changeCraft()
	{
		std::vector<Craft*> *crafts = _base->getCrafts();

		if (crafts->empty())
		{
			return;
		}

		std::vector<Soldier*> selectedSoldiers = getSelectedSoldiers();

		if (selectedSoldiers.empty())
		{
			return;
		}

		Craft *craftToAssign = 0;

		if (checkSameCraftAssignments())
		{
			// move on to the next possible craft selection; if there's only 1, then remove the assignment
			if (crafts->size() > 1)
			{
				Craft *tempCraft = 0;
				
				// if this is the first time iterating through the list for this group, set it up
				if (_curCraftInList == -2)
				{
					tempCraft = (*selectedSoldiers.begin())->getCraft();
					_curCraftInList = findCraftAmongstAllCrafts(tempCraft);
				}

				bool done = false;

				// loop through until we find a desirable craft to assign
				while (!done)
				{
					_curCraftInList++;

					// assign to "NONE" if we've reached the end of the list
					if (_curCraftInList == crafts->size())
					{
						_curCraftInList = -1;
						craftToAssign = 0;
						done = true;
					}
					else
					{
						tempCraft = crafts->at(_curCraftInList);

						// TODO: Put in code to handle if ship is currently out (can't select those soldiers?).  Ditto for wounded soldiers.
						// TODO: Test assigning soldiers to ship if the destination ship is out.
						if (tempCraft->getSpaceAvailable() >= selectedSoldiers.size())
						{
							craftToAssign = tempCraft;
							done = true;
						}
					}
				}
			}
		}
		else
		{
			// find the first craft for the selected soldiers and assign it to all selected soldiers
			craftToAssign = (*selectedSoldiers.begin())->getCraft();
			_curCraftInList = findCraftAmongstAllCrafts(craftToAssign);
		}

		std::map<Soldier*, size_t> soldiersWithRows = getSelectedSoldiersWithRows();
		std::wstring craftName = (craftToAssign == 0 ? tr("STR_NONE_UC") : craftToAssign->getName(_game->getLanguage()));

		for (std::vector<Soldier*>::iterator it = selectedSoldiers.begin(); it != selectedSoldiers.end(); ++it)
		{
			(*it)->setCraft(craftToAssign);
			_lstSoldiers->setCellText(soldiersWithRows[(*it)], 2, craftName);
		}
	}

	/**
	  * Finds the passed-in craft amongst the overall crafts.
	  * @param craft The craft to find.
	  * @return int Returns the index in the vector if craft is found; -1 otherwise.
	  */
	int SoldiersState::findCraftAmongstAllCrafts(Craft *craft)
	{
		int index = -1;

		std::vector<Craft*> *crafts = _base->getCrafts();

		if (crafts->empty())
		{
			return index;
		}

		for (int i = 0; i < crafts->size(); ++i)
		{			
			if (crafts->at(i) == craft)
			{
				index = i;
				break;
			}
		}

		return index;
	}

	/**
	  * Check if all selected soldiers are on the same craft.
	  * @return bool True iff all selected soldiers belong to the same craft.
	  */
	bool SoldiersState::checkSameCraftAssignments()
	{
		std::vector<Soldier*> selectedSoldiers = getSelectedSoldiers();
		bool allSame = true;

		if (selectedSoldiers.size() > 1)
		{
			Craft *craft = (*selectedSoldiers.begin())->getCraft();

			for (std::vector<Soldier*>::iterator it = ++(selectedSoldiers.begin()); it != selectedSoldiers.end(); ++it)
			{
				if ((*it)->getCraft() != craft)
				{
					allSame = false;
					break;
				}
			}			
		}
		
		return allSame;
	}

	/**
	  * Sends back a vector of all soldiers corresponding to the selections made in the list.
	  * @return std::vector<Soldier*> Vector of all selected soldiers.
	  */
	std::vector<Soldier*> SoldiersState::getSelectedSoldiers()
	{
		std::vector<Soldier*> selectedSoldiers;
		std::vector<size_t> selectedRows = _lstSoldiers->getSelectedRows();

		for (std::vector<size_t>::iterator it = selectedRows.begin(); it != selectedRows.end(); ++it)
		{
			selectedSoldiers.push_back(_base->getSoldiers()->at(*it));
		}
		
		return selectedSoldiers;
	}

	/**
	* Sends back a map of all soldiers with associated row numbers corresponding to the selections made in the list.
	* @return std::map<Soldier*, size_t> Map of all selected soldiers along with respective row numbers.
	*/
	std::map<Soldier*, size_t> SoldiersState::getSelectedSoldiersWithRows()
	{
		std::map<Soldier*, size_t> selectedSoldiers;
		std::vector<size_t> selectedRows = _lstSoldiers->getSelectedRows();

		for (std::vector<size_t>::iterator it = selectedRows.begin(); it != selectedRows.end(); ++it)
		{
			selectedSoldiers[_base->getSoldiers()->at(*it)] = *it;
		}

		return selectedSoldiers;
	}
}