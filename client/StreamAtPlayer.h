/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <memory>
#include <string>

#include <d3d9.h>

#include <audio/bass.h>
#include <samp/CNetGame.h>
#include <game/CPed.h>

#include "StreamInfo.h"
#include "LocalStream.h"
#include "Channel.h"

class StreamAtPlayer : public LocalStream {

    StreamAtPlayer() = delete;
    StreamAtPlayer(const StreamAtPlayer&) = delete;
    StreamAtPlayer(StreamAtPlayer&&) = delete;
    StreamAtPlayer& operator=(const StreamAtPlayer&) = delete;
    StreamAtPlayer& operator=(StreamAtPlayer&&) = delete;

public:

    explicit StreamAtPlayer(PlayHandlerType&& playHandler, StopHandlerType&& stopHandler,
                            const std::string& name, const D3DCOLOR color,
                            const WORD playerId, const float distance)
        : LocalStream(BASS_SAMPLE_3D | BASS_SAMPLE_MUTEMAX,
                      std::move(playHandler), std::move(stopHandler),
                      StreamType::LocalStreamAtPlayer,
                      name, color, distance)
        , playerId(playerId) {}

    ~StreamAtPlayer() noexcept = default;

public:

    void Tick() noexcept override
    {
        this->LocalStream::Tick();

        const auto pNetGame = SAMP::pNetGame();
        if (!pNetGame) return;

        const auto pPlayerPool = pNetGame->GetPlayerPool();
        if (!pPlayerPool) return;

        const auto pPlayer = pPlayerPool->GetPlayer(this->playerId);
        if (!pPlayer) return;

        const auto pPlayerPed = pPlayer->m_pPed;
        if (!pPlayerPed) return;

        const auto pPlayerGamePed = pPlayerPed->m_pGamePed;
        if (!pPlayerGamePed) return;

        const auto pPlayerMatrix = pPlayerGamePed->GetMatrix();
        if (!pPlayerMatrix) return;

        for (const auto& iChan : this->channels)
        {
            if (iChan->speaker != SV::NonePlayer)
            {
                BASS_ChannelSet3DPosition(iChan->handle,
                    reinterpret_cast<BASS_3DVECTOR*>(&pPlayerMatrix->pos),
                    nullptr, nullptr);
            }
        }
    }

private:

    void ChannelCreationHandler(const Channel& channel) noexcept override
    {
        static const BASS_3DVECTOR kZeroVector { 0, 0, 0 };

        this->LocalStream::ChannelCreationHandler(channel);

        const auto pNetGame = SAMP::pNetGame();
        if (!pNetGame) return;

        const auto pPlayerPool = pNetGame->GetPlayerPool();
        if (!pPlayerPool) return;

        const auto pPlayer = pPlayerPool->GetPlayer(this->playerId);
        if (!pPlayer) return;

        const auto pPlayerPed = pPlayer->m_pPed;
        if (!pPlayerPed) return;

        const auto pPlayerGamePed = pPlayerPed->m_pGamePed;
        if (!pPlayerGamePed) return;

        const auto pPlayerMatrix = pPlayerGamePed->GetMatrix();
        if (!pPlayerMatrix) return;

        BASS_ChannelSet3DPosition(channel.handle,
            reinterpret_cast<BASS_3DVECTOR*>(&pPlayerMatrix->pos),
            &kZeroVector, &kZeroVector);
    }

private:

    const WORD playerId { NULL };

};

using StreamAtPlayerPtr = std::shared_ptr<StreamAtPlayer>;
#define MakeStreamAtPlayer std::make_shared<StreamAtPlayer>
