/*
 * This file is part of the AzerothCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ChannelMgr.h"
#include "GlobalChatMgr.h"
#include "ScriptMgr.h"
#include "StringFormat.h"

class GlobalChat_Config : public WorldScript
{
public: 
    GlobalChat_Config() : WorldScript("GlobalChat_Config") { };

    void OnAfterConfigLoad(bool reload) override
    {
        sGlobalChatMgr->LoadConfig(reload);
    }
};

class GlobalChat_Player : public PlayerScript
{
public:
    GlobalChat_Player() : PlayerScript("GlobalChat_Player") { }

    void OnLogin(Player* player) override
    {
        if (sGlobalChatMgr->GlobalChatEnabled)
        {
            if (sGlobalChatMgr->Announce)
            {
                ChatHandler(player->GetSession()).SendSysMessage("This server is running the |cff4CFF00GlobalChat|r module. Use |cff4CFF00.help global|r to find out how to use it.");
            }

            sGlobalChatMgr->LoadPlayerData(player);

            if (!sGlobalChatMgr->IsInChat(player->GetGUID()))
            {
                if (sGlobalChatMgr->JoinChannel && !sGlobalChatMgr->ChatName.empty())
                {
                    ChatHandler(player->GetSession()).SendSysMessage(Acore::StringFormat("You can join the |cffFF0000GlobalChat|r by typing |cffFF0000.joinglobal|r or |cffFF0000/join {}|r at any time.", sGlobalChatMgr->ChatName));
                }
                else
                {
                    ChatHandler(player->GetSession()).SendSysMessage("You can join the |cffFF0000GlobalChat|r by typing |cffFF0000.joinglobal|r at any time.");
                }
            }
        }
    }

    void OnSave(Player* player) override
    {
        sGlobalChatMgr->SavePlayerData(player);
    }

    void OnChat(Player* player, uint32 /*type*/, uint32 lang, std::string& msg, Channel* channel) override
    {
        if (sGlobalChatMgr->JoinChannel && !sGlobalChatMgr->ChatName.empty() && lang != LANG_ADDON && channel->GetName() == sGlobalChatMgr->ChatName)
        {
            if (sGlobalChatMgr->FactionSpecific && player->GetSession()->GetSecurity() > 0)
            {
                ChatHandler(player->GetSession()).SendSysMessage("Please use |cff4CFF00.galliance|r or .|cff4CFF00ghorde|r for the GlobalChat as GM.");
                msg = "";
                return;
            }

            sGlobalChatMgr->SendGlobalChat(player->GetSession(), msg);
            msg = "";
        }
    }
};

void AddSC_GlobalChat()
{
    new GlobalChat_Config();
    new GlobalChat_Player();
}