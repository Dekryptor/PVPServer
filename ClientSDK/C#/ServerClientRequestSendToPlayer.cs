﻿using System;

public partial class ServerClient : NetClient
{
    public virtual void RequestSendToPlayer(uint guid, int size, byte[] data)
    {
        mRequestSendToPlayer.guid = guid;
        mRequestSendToPlayer.size = size;
        mRequestSendToPlayer.data = data;
        SendProto(Client.REQUEST_MSG.SEND_TO_PLAYER, mRequestSendToPlayer);
    }

    public virtual void RequestSendToPlayerAll(uint filter, int size, byte[] data)
    {
        mRequestSendToPlayerAll.filter = filter;
        mRequestSendToPlayerAll.size = size;
        mRequestSendToPlayerAll.data = data;
        SendProto(Client.REQUEST_MSG.SEND_TO_PLAYER_ALL, mRequestSendToPlayerAll);
    }

    public virtual void ResponseSendToPlayer(byte[] buffer)
    {
        Server.SendToPlayer responseSendToPlayer = ProtoHelper.ToProto<Server.SendToPlayer>(buffer);

        string text = System.Text.Encoding.Default.GetString(responseSendToPlayer.data);
        Console.WriteLine(text);

        if (onResponseSendToPlayer != null)
        {
            onResponseSendToPlayer();
        }
    }
}
