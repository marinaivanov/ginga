/******************************************************************************
Este arquivo eh parte da implementacao do ambiente declarativo do middleware
Ginga (Ginga-NCL).

Direitos Autorais Reservados (c) 1989-2007 PUC-Rio/Laboratorio TeleMidia

Este programa eh software livre; voce pode redistribui-lo e/ou modificah-lo sob
os termos da Licenca Publica Geral GNU versao 2 conforme publicada pela Free
Software Foundation.

Este programa eh distribuido na expectativa de que seja util, porem, SEM
NENHUMA GARANTIA; nem mesmo a garantia implicita de COMERCIABILIDADE OU
ADEQUACAO A UMA FINALIDADE ESPECIFICA. Consulte a Licenca Publica Geral do
GNU versao 2 para mais detalhes.

Voce deve ter recebido uma copia da Licenca Publica Geral do GNU versao 2 junto
com este programa; se nao, escreva para a Free Software Foundation, Inc., no
endereco 59 Temple Street, Suite 330, Boston, MA 02111-1307 USA.

Para maiores informacoes:
ncl @ telemidia.puc-rio.br
http://www.ncl.org.br
http://www.ginga.org.br
http://www.telemidia.puc-rio.br
******************************************************************************
This file is part of the declarative environment of middleware Ginga (Ginga-NCL)

Copyright: 1989-2007 PUC-RIO/LABORATORIO TELEMIDIA, All Rights Reserved.

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License version 2 as published by
the Free Software Foundation.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License version 2 for more
details.

You should have received a copy of the GNU General Public License version 2
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA

For further information contact:
ncl @ telemidia.puc-rio.br
http://www.ncl.org.br
http://www.ginga.org.br
http://www.telemidia.puc-rio.br
*******************************************************************************/

#include "gingancl/adapters/PlayerAdapterManager.h"

#include "gingancl/adapters/AdaptersComponentSupport.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace adapters {
	PlayerAdapterManager::PlayerAdapterManager(NclPlayerData* data) : Thread() {
		nclPlayerData = data;

		editingCommandListener = NULL;
		epgFactoryAdapter      = NULL;
		timeBaseProvider       = NULL;

		Thread::mutexInit(&mutexPlayer, NULL);

		readConfigFiles();

		running = true;
		startThread();
	}

	PlayerAdapterManager::~PlayerAdapterManager() {
		isDeleting = true;
		running    = false;
		unlockConditionSatisfied();
		wakeUp();

		clear();
		clearDeletePlayers();

		Thread::mutexLock(&mutexPlayer);
		Thread::mutexUnlock(&mutexPlayer);
		Thread::mutexDestroy(&mutexPlayer);

		clog << "PlayerAdapterManager::~PlayerAdapterManager all done" << endl;
	}

	bool PlayerAdapterManager::hasPlayer(IPlayerAdapter* player) {
		bool hasInstance = false;

		Thread::mutexLock(&mutexPlayer);
		if (playerNames.find(player) != playerNames.end()) {
			hasInstance = true;
		}
		Thread::mutexUnlock(&mutexPlayer);

		return hasInstance;
	}

	NclPlayerData* PlayerAdapterManager::getNclPlayerData() {
		return nclPlayerData;
	}

	void PlayerAdapterManager::setTimeBaseProvider(
			ITimeBaseProvider* timeBaseProvider) {

		this->timeBaseProvider = timeBaseProvider;
	}

	ITimeBaseProvider* PlayerAdapterManager::getTimeBaseProvider() {
		return timeBaseProvider;
	}

	void PlayerAdapterManager::setVisible(
			string objectId,
			string visible,
			AttributionEvent* event) {

		map<string, IPlayerAdapter*>::iterator i;
		FormatterPlayerAdapter* player;

		Thread::mutexLock(&mutexPlayer);
		i = objectPlayers.find(objectId);
		if (i != objectPlayers.end()) {
			player = (FormatterPlayerAdapter*)(i->second);
			player->setPropertyValue(event, visible);
			event->stop();
		}
		Thread::mutexUnlock(&mutexPlayer);
	}

	bool PlayerAdapterManager::removePlayer(void* exObject) {
		ExecutionObject* object;
		bool removed = false;
		string objId;

		object = (ExecutionObject*)exObject;
		Thread::mutexLock(&mutexPlayer);
		if (ExecutionObject::hasInstance(object, false)) {
			objId   = object->getId();
			removed = removePlayer(objId);
		}
		Thread::mutexUnlock(&mutexPlayer);

		return removed;
	}

	bool PlayerAdapterManager::removePlayer(string objectId) {
		map<string, IPlayerAdapter*>::iterator i;
		FormatterPlayerAdapter* player;

		i = objectPlayers.find(objectId);
		if (i != objectPlayers.end()) {
			player = (FormatterPlayerAdapter*)(i->second);
			if (!player->instanceOf("ProgramAVPlayerAdapter")) {
				deletePlayers[objectId] = player;
			}
			objectPlayers.erase(i);
			unlockConditionSatisfied();
			return true;
		}

		return false;
	}

	void PlayerAdapterManager::clear() {
		map<string, IPlayerAdapter*>::iterator i;

		Thread::mutexLock(&mutexPlayer);
		i = objectPlayers.begin();
		while (i != objectPlayers.end()) {
			if (removePlayer(i->first)) {
				i = objectPlayers.begin();
			} else {
				++i;
			}
		}
		objectPlayers.clear();
		Thread::mutexUnlock(&mutexPlayer);
	}

	void PlayerAdapterManager::setNclEditListener(IPlayerListener* listener) {
		this->editingCommandListener = listener;
	}

	string PlayerAdapterManager::getPlayerClass(
		    CascadingDescriptor* descriptor, NodeEntity* dataObject) {

		string symName   = "";
		string toolName  = "";
		string pToolName = "";
		string mime      = "";
		string upMime    = "";
		string nodeType  = "";
		string url       = "";

		std::string::size_type pos;
		Content* content;

		if (descriptor != NULL) {
			toolName = descriptor->getPlayerName();
		}

		if (dataObject->instanceOf("ContentNode")) {
			mime = ((ContentNode*)dataObject)->getNodeType();
			if (mime == "" && toolName == "") {
				content = dataObject->getContent();
				if (content != NULL &&
						content->instanceOf("ReferenceContent")) {

					url = ((ReferenceContent*)(content))->
						    getCompleteReferenceUrl();

					mime = getMimeTypeFromSchema(url);

				} else {
					clog << "PlayerAdapterManager::getPlayerClass can't ";
					clog << "define MIME type. Content is NULL or not an ";
					clog << "instance of ReferenceContent";
					clog << endl;
				}

				if (mime == "") {
					clog << "PlayerAdapterManager::getPlayerClass can't ";
					clog << "define MIME type. Creating a time player for ";
					clog << "URL '" << url << "'";
					clog << endl;
					return "";
				}
			}

			if (((ContentNode*)dataObject)->isSettingNode()) {
				return "SETTING_NODE";
			}
		}

		if (toolName == "") {
			/*
			 *  there is no player defined!
			 *  lets choose a player based on the node content type
			 */
			upMime = upperCase(mime);
			if (mimeDefaultTable.count(upMime) != 0) {
				toolName = mimeDefaultTable[upMime];
			}

			clog << "PlayerAdapterManager::getPlayerClass is ";
			clog << "finding a suitable player" << endl;

			if (toolName != "") {
				/*
				 *  returning player considering the defined priority order
				 *     for instance: first::second::third ...
				 */
				pToolName = "";
				while (toolName != "") {
					if (toolName.find("::") != std::string::npos) {
						pos       = toolName.find_first_of("::");
						pToolName = toolName.substr(0, pos);
						toolName  = toolName.substr(
								pos + 2, toolName.length() - pos);

					} else if (toolName == pToolName) {
						toolName  = "";
						pToolName = "";

					} else {
						pToolName = toolName;
					}

#if HAVE_COMPONENTS
					pos = pToolName.find_first_of(",");
					if (pos != std::string::npos) {
						symName = pToolName.substr(0, pos);

					} else {
						symName = pToolName;
					}

					if (cm->isAvailable(symName)) {
						clog << "PlayerAdapterManager::getPlayerClass '";
						clog << symName << "' is available!";
						clog << endl;
						toolName = "";
					}
#else
					break;
#endif
				}
				return pToolName;
			}

		} else {
			if (playerTable.count(toolName)) {
				return playerTable[toolName];
			}
		}

		toolName = "";
		return toolName;
	}

	void PlayerAdapterManager::readConfigFiles() {
		ifstream fisMime;
		ifstream fisCtrl;
		string line, key, value;

		string mimeUri;
		string ctrlUri;

		mimeUri = SystemCompat::appendGingaFilesPrefix("cfg/formatter/mimedefs.ini");
		ctrlUri = SystemCompat::appendGingaFilesPrefix("cfg/formatter/ctrldefs.ini");

		fisMime.open(mimeUri.c_str(), ifstream::in);

		if (!fisMime.is_open()) {
			clog << "PlayerAdapterManager::readConfigFiles Warning! Can't open '";
			clog << mimeUri << "'" << endl;
			return;
		}

		mimeDefaultTable.clear();
		while (fisMime.good()) {
			fisMime >> line;
			if (line.substr(0, 1) != "#") {
				key = upperCase(line.substr(0, line.find_last_of("=")));
				value = line.substr(
						(line.find_first_of("=") + 1),
						line.length() - (line.find_first_of("=") + 1));

				mimeDefaultTable[key] = value;
			}
		}

		fisMime.close();

		fisCtrl.open(ctrlUri.c_str());
		if (!fisCtrl.is_open()) {
			clog << "PlayerAdapterManager::readConfigFiles Warning! Can't open '";
			clog << fisCtrl << "'" << endl;
			return;
		}

		playerTable.clear();
		while (fisCtrl.good()) {
			fisCtrl >> line;
			if (line.substr(0, 1) != "#") {
				key = line.substr(0, line.find_last_of("="));
				value = line.substr(
						(line.find_first_of("=") + 1),
						line.length() - (line.find_first_of("=") + 1));

				playerTable[key] = value;
			}
		}

		fisCtrl.close();
	}

	FormatterPlayerAdapter* PlayerAdapterManager::initializePlayer(
		    ExecutionObject* object) {

		CascadingDescriptor* descriptor;
		NodeEntity* dataObject;
		string playerClassName, objId;
		IPlayerAdapter* player = NULL;
		vector<string>* args;
		void* compObject;
		string param = "";

		if (object == NULL) {
			return NULL;
		}

		objId = object->getId();
		descriptor = object->getDescriptor();
		dataObject = (NodeEntity*)(object->getDataObject()->getDataEntity());

		// checking if is a main AV reference
		Content* content;
		string url = "";

		playerClassName = "";
		content = dataObject->getContent();
		if (content != NULL) {
			if (content->instanceOf("ReferenceContent")) {
				url = ((ReferenceContent*)(content))->
					    getCompleteReferenceUrl();

				if (url.length() > 9 && url.substr(0,9) == "sbtvd-ts:") {
					playerClassName = "ProgramAVPlayerAdapter";

				} else if (url.length() > 13 && url.substr(0,13) == "ncl-mirror://") {
					playerClassName = "MirrorPlayerAdapter";
				}
			}
		}

		if (playerClassName == "") {
			playerClassName = getPlayerClass(descriptor, dataObject);
		}

		if (playerClassName == "SETTING_NODE") {
			return NULL;
		}

		if (playerClassName == "") {
			clog << "PlayerAdapterManager::initializePlayer creating ";
			clog << "LOCAL TIME player" << endl;
			player = new FormatterPlayerAdapter(this);
			objectPlayers[objId] = player;
			return (FormatterPlayerAdapter*)player;
		}

		args = split(playerClassName, ",");
		if (args->size() < 1) {
			delete args;
			return NULL;

		} else if (args->size() == 1) {
			args->push_back("");
		}

		playerClassName = (*args)[0];

#if HAVE_COMPONENTS
		compObject = cm->getObject(playerClassName);

		if (compObject != NULL) {
			player = ((AdapterCreator*)compObject)(
					this, (void*)(((*args)[1]).c_str()));
		}
#else
		if (playerClassName == "SubtitlePlayerAdapter") {
			player = new SubtitlePlayerAdapter(this);

		} else if (playerClassName == "PlainTxtPlayerAdapter") {
			player = new PlainTxtPlayerAdapter(this);

		} else if (playerClassName == "AwesomiumPlayerAdapter") {
#if HAVE_AWESOMIUM
			player = new AwesomiumPlayerAdapter(this);
#endif //HAVE_AWESOMIUM

		} else if (playerClassName == "BerkeliumPlayerAdapter") {
#if HAVE_BERKELIUM
			player = new BerkeliumPlayerAdapter(this);
#endif //HAVE_BERKELIUM

		} else if (playerClassName == "LinksPlayerAdapter") {
#if HAVE_LINKS
			player = new LinksPlayerAdapter(this);
#endif //HAVE_LINKS

		} else if (playerClassName == "ImagePlayerAdapter") {
			player = new ImagePlayerAdapter(this);

		} else if (playerClassName == "MirrorPlayerAdapter") {
			player = new MirrorPlayerAdapter(this);

		} else if (playerClassName == "AVPlayerAdapter") {
			if ((*args)[1] == "true") {
				player = new AVPlayerAdapter(this, true);
			} else {
				player = new AVPlayerAdapter(this, false);
			}

		} else if (playerClassName == "LuaPlayerAdapter") {
			player = new LuaPlayerAdapter(this);

		} else if (playerClassName == "NCLPlayerAdapter") {
			player = new NCLPlayerAdapter(this);

		} else if (playerClassName == "ChannelPlayerAdapter") {
			if ((*args)[1] == "true") {
				player = new ChannelPlayerAdapter(this, true);
			} else {
				player = new ChannelPlayerAdapter(this, false);
			}

		} else if (playerClassName == "ProgramAVPlayerAdapter") {
			player = ProgramAVPlayerAdapter::getInstance(this);

		} else if (playerClassName != "SETTING_NODE") {
			clog << "PlayerAdapterManager::initializePlayer is creating a ";
			clog << "new time player for '" << objId << "'";
			clog << " playerClassName is '" << playerClassName;
			clog << "'" << endl;
			player = new FormatterPlayerAdapter(this);

		} else {
			clog << "PlayerAdapterManager::initializePlayer is returning a ";
			clog << "NULL player for '" << objId << "'" << endl;
			delete args;
			return NULL;
		}
#endif

		if (player == NULL) {
			delete args;
			return NULL;
		}

		param = (*args)[1];
		if (param == "epg") {
			epgFactoryAdapter = player;
		}

		if (param == "epg" || param == "nclEdit") {
			((FormatterPlayerAdapter*)player)->setNclEditListener(
					editingCommandListener);
		}

		delete args;

		objectPlayers[objId] = player;
		playerNames[player]  = playerClassName;

		clog << "PlayerAdapterManager::initializePlayer creating '";
		clog << playerClassName << "' param = '" << param << "'";
		clog << endl;

		return (FormatterPlayerAdapter*)player;
	}

	FormatterPlayerAdapter* PlayerAdapterManager::getObjectPlayer(
		    ExecutionObject* execObj) {

		map<string, IPlayerAdapter*>::iterator i;
		FormatterPlayerAdapter* player;
		string objId;

		Thread::mutexLock(&mutexPlayer);
		objId = execObj->getId();
		i = objectPlayers.find(objId);
		if (i == objectPlayers.end()) {
			i = deletePlayers.find(objId);
			if (i == deletePlayers.end()) {
				player = initializePlayer(execObj);
			} else {
				player = (FormatterPlayerAdapter*)(i->second);
				deletePlayers.erase(i);
				objectPlayers[objId] = player;
			}

		} else {
			player = (FormatterPlayerAdapter*)(i->second);
		}
		Thread::mutexUnlock(&mutexPlayer);

		return player;
	}

	string PlayerAdapterManager::getMimeTypeFromSchema(string url) {
		string mime = "";

		if ((url.length() > 8 && url.substr(0, 8) == "https://") ||
				(url.length() > 7 && url.substr(0, 7) == "http://") ||
				(url.length() > 4 && url.substr(0, 4) == "www.")) {

			clog << "PlayerAdapterManager::getMimeTypeFromSchema is ";
			clog << "considering HTML MIME." << endl;

			mime = ContentTypeManager::getInstance()->getMimeType("html");

		} else if ((url.length() > 6 && url.substr(0, 6) == "rtp://") ||
				(url.length() > 7 && url.substr(0, 7) == "rtsp://")) {

			mime = ContentTypeManager::getInstance()->getMimeType("mpg");
		}

		return mime;
	}

	/*void PlayerAdapterManager::setStandByState(bool standBy) {
		map<string, IPlayerAdapter*>::iterator i;
		FormatterPlayerAdapter* player;

		Thread::mutexLock(&mutexPlayer);
		i = objectPlayers.begin();
		while (i != objectPlayers.end()) {
			player = (FormatterPlayerAdapter*)(i->second);
			if (player->getPlayer() != NULL) {
				player->getPlayer()->setStandByState(standBy);
			}
			++i;
		}
		Thread::mutexUnlock(&mutexPlayer);
	}*/

	bool PlayerAdapterManager::isEmbeddedApp(NodeEntity* dataObject) {
		string mediaType = "";
		string url = "";
		string::size_type pos;
		Descriptor* descriptor;
		Content* content;

		//first, descriptor
		descriptor = (Descriptor*)(dataObject->getDescriptor());
		if (descriptor != NULL && !descriptor->instanceOf("DescriptorSwitch")) {
			mediaType = descriptor->getPlayerName();
			if (mediaType == "NCLetPlayerAdapter" ||
					mediaType == "LuaPlayerAdapter" ||
					mediaType == "AwesomiumPlayerAdapter" ||
					mediaType == "BerkeliumPlayerAdapter" ||
					mediaType == "LinksPlayerAdapter" ||
					mediaType == "NCLPlayerAdapter") {

				return true;
			}
		}

		//second, media type
		if (dataObject->instanceOf("ContentNode")) {
			mediaType = ((ContentNode*)dataObject)->getNodeType();
			if (mediaType != "") {
				return isEmbeddedAppMediaType(mediaType);
			}
		}

		//finally, content file extension
		content = dataObject->getContent();
		if (content != NULL) {
			if (content->instanceOf("ReferenceContent")) {
				url = ((ReferenceContent*)(content))->
					    getCompleteReferenceUrl();

				if (url != "") {
					pos = url.find_last_of(".");
					if (pos != std::string::npos) {
						pos++;
						mediaType = ContentTypeManager::getInstance()->
								getMimeType(url.substr(
										pos, url.length() - pos));

						return isEmbeddedAppMediaType(mediaType);
					}
				}
			}
		}

		return false;
	}

	bool PlayerAdapterManager::isEmbeddedAppMediaType(string mediaType) {
		string upMediaType = upperCase(mediaType);

		if (upMediaType == "APPLICATION/X-GINGA-NCLUA" ||
				upMediaType == "APPLICATION/X-GINGA-NCLET" ||
				upMediaType == "APPLICATION/X-GINGA-NCL" ||
				upMediaType == "APPLICATION/X-NCL-NCL" ||
				upMediaType == "APPLICATION/X-NCL-NCLUA") {

			return true;
		}

		return false;
	}

	void PlayerAdapterManager::timeShift(string direction) {
		map<string, IPlayerAdapter*>::iterator i;
		FormatterPlayerAdapter* player;

		Thread::mutexLock(&mutexPlayer);
		i = objectPlayers.begin();
		while (i != objectPlayers.end()) {
			player = (FormatterPlayerAdapter*)(i->second);
			player->timeShift(direction);
			++i;
		}
		Thread::mutexUnlock(&mutexPlayer);
	}

	void PlayerAdapterManager::clearDeletePlayers() {
		map<string, IPlayerAdapter*> dPlayers;

		map<string, IPlayerAdapter*>::iterator i;
		map<IPlayerAdapter*, string>::iterator j;
		IPlayerAdapter* player;
		string playerClassName = "";

		Thread::mutexLock(&mutexPlayer);
		i = deletePlayers.begin();
		while (i != deletePlayers.end()) {
			player = i->second;

			j = playerNames.find(player);
			if (j != playerNames.end()) {
				playerClassName = j->second;
				playerNames.erase(j);
			}

			if (((FormatterPlayerAdapter*)player)->getObjectDevice() == 0) {
				dPlayers[playerClassName] = player;
			}

			++i;
		}
		deletePlayers.clear();
		Thread::mutexUnlock(&mutexPlayer);

		i = dPlayers.begin();
		while (i != dPlayers.end()) {
			player = i->second;
			playerClassName = i->first;

			delete player;

#if HAVE_COMPONENTS
			if (trim(playerClassName) != "") {
				cm->releaseComponentFromObject(playerClassName);
			}
#endif
			++i;
		}
	}

	void PlayerAdapterManager::run() {
		set<IPlayerAdapter*>::iterator i;

		while(running) {
			if (!isDeleting) {
				Thread::mutexLock(&mutexPlayer);
				if (deletePlayers.empty()) {
					Thread::mutexUnlock(&mutexPlayer);
					waitForUnlockCondition();

				} else {
					Thread::mutexUnlock(&mutexPlayer);
				}
			}

			if (!running) {
				return;
			}

			if (isDeleting) {
				break;
			}

			Thread::mSleep(1000);
			if (running) {
				clearDeletePlayers();
			}
		}

		clog << "PlayerAdapterManager::run all done" << endl;
	}
}
}
}
}
}
}