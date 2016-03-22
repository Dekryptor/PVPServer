//------------------------------------------------------------------------------
// <auto-generated>
//     This code was generated by a tool.
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
// </auto-generated>
//------------------------------------------------------------------------------

// Generated from: ProtoGateServer.proto
namespace ProtoGateServer
{
  [global::System.Serializable, global::ProtoBuf.ProtoContract(Name=@"Heart")]
  public partial class Heart : global::ProtoBuf.IExtensible
  {
    public Heart() {}
    
    private int _timestamp;
    [global::ProtoBuf.ProtoMember(1, IsRequired = true, Name=@"timestamp", DataFormat = global::ProtoBuf.DataFormat.TwosComplement)]
    public int timestamp
    {
      get { return _timestamp; }
      set { _timestamp = value; }
    }
    private global::ProtoBuf.IExtension extensionObject;
    global::ProtoBuf.IExtension global::ProtoBuf.IExtensible.GetExtensionObject(bool createIfMissing)
      { return global::ProtoBuf.Extensible.GetExtensionObject(ref extensionObject, createIfMissing); }
  }
  
  [global::System.Serializable, global::ProtoBuf.ProtoContract(Name=@"GameServerList")]
  public partial class GameServerList : global::ProtoBuf.IExtensible
  {
    public GameServerList() {}
    
    private readonly global::System.Collections.Generic.List<ProtoGateServer.GameServerList.GameServer> _servers = new global::System.Collections.Generic.List<ProtoGateServer.GameServerList.GameServer>();
    [global::ProtoBuf.ProtoMember(1, Name=@"servers", DataFormat = global::ProtoBuf.DataFormat.Default)]
    public global::System.Collections.Generic.List<ProtoGateServer.GameServerList.GameServer> servers
    {
      get { return _servers; }
    }
  
  [global::System.Serializable, global::ProtoBuf.ProtoContract(Name=@"GameServer")]
  public partial class GameServer : global::ProtoBuf.IExtensible
  {
    public GameServer() {}
    
    private string _ip;
    [global::ProtoBuf.ProtoMember(1, IsRequired = true, Name=@"ip", DataFormat = global::ProtoBuf.DataFormat.Default)]
    public string ip
    {
      get { return _ip; }
      set { _ip = value; }
    }
    private int _port;
    [global::ProtoBuf.ProtoMember(2, IsRequired = true, Name=@"port", DataFormat = global::ProtoBuf.DataFormat.TwosComplement)]
    public int port
    {
      get { return _port; }
      set { _port = value; }
    }
    private readonly global::System.Collections.Generic.List<ProtoGateServer.GameServerList.GameServer.Game> _games = new global::System.Collections.Generic.List<ProtoGateServer.GameServerList.GameServer.Game>();
    [global::ProtoBuf.ProtoMember(3, Name=@"games", DataFormat = global::ProtoBuf.DataFormat.Default)]
    public global::System.Collections.Generic.List<ProtoGateServer.GameServerList.GameServer.Game> games
    {
      get { return _games; }
    }
  
  [global::System.Serializable, global::ProtoBuf.ProtoContract(Name=@"Game")]
  public partial class Game : global::ProtoBuf.IExtensible
  {
    public Game() {}
    
    private bool _private;
    [global::ProtoBuf.ProtoMember(1, IsRequired = true, Name=@"private", DataFormat = global::ProtoBuf.DataFormat.Default)]
    public bool @private
    {
      get { return _private; }
      set { _private = value; }
    }
    private int _gameid;
    [global::ProtoBuf.ProtoMember(2, IsRequired = true, Name=@"gameid", DataFormat = global::ProtoBuf.DataFormat.TwosComplement)]
    public int gameid
    {
      get { return _gameid; }
      set { _gameid = value; }
    }
    private int _mode;
    [global::ProtoBuf.ProtoMember(3, IsRequired = true, Name=@"mode", DataFormat = global::ProtoBuf.DataFormat.TwosComplement)]
    public int mode
    {
      get { return _mode; }
      set { _mode = value; }
    }
    private int _map;
    [global::ProtoBuf.ProtoMember(4, IsRequired = true, Name=@"map", DataFormat = global::ProtoBuf.DataFormat.TwosComplement)]
    public int map
    {
      get { return _map; }
      set { _map = value; }
    }
    private int _maxPlayers;
    [global::ProtoBuf.ProtoMember(5, IsRequired = true, Name=@"maxPlayers", DataFormat = global::ProtoBuf.DataFormat.TwosComplement)]
    public int maxPlayers
    {
      get { return _maxPlayers; }
      set { _maxPlayers = value; }
    }
    private readonly global::System.Collections.Generic.List<int> _playes = new global::System.Collections.Generic.List<int>();
    [global::ProtoBuf.ProtoMember(6, Name=@"playes", DataFormat = global::ProtoBuf.DataFormat.TwosComplement)]
    public global::System.Collections.Generic.List<int> playes
    {
      get { return _playes; }
    }
  
    private global::ProtoBuf.IExtension extensionObject;
    global::ProtoBuf.IExtension global::ProtoBuf.IExtensible.GetExtensionObject(bool createIfMissing)
      { return global::ProtoBuf.Extensible.GetExtensionObject(ref extensionObject, createIfMissing); }
  }
  
    private global::ProtoBuf.IExtension extensionObject;
    global::ProtoBuf.IExtension global::ProtoBuf.IExtensible.GetExtensionObject(bool createIfMissing)
      { return global::ProtoBuf.Extensible.GetExtensionObject(ref extensionObject, createIfMissing); }
  }
  
    private global::ProtoBuf.IExtension extensionObject;
    global::ProtoBuf.IExtension global::ProtoBuf.IExtensible.GetExtensionObject(bool createIfMissing)
      { return global::ProtoBuf.Extensible.GetExtensionObject(ref extensionObject, createIfMissing); }
  }
  
    [global::ProtoBuf.ProtoContract(Name=@"VERSION_NUMBER")]
    public enum VERSION_NUMBER
    {
            
      [global::ProtoBuf.ProtoEnum(Name=@"VERSION", Value=65536)]
      VERSION = 65536
    }
  
    [global::ProtoBuf.ProtoContract(Name=@"RESPONSE_MSG")]
    public enum RESPONSE_MSG
    {
            
      [global::ProtoBuf.ProtoEnum(Name=@"HEART", Value=3000)]
      HEART = 3000,
            
      [global::ProtoBuf.ProtoEnum(Name=@"GAME_SERVER_LIST", Value=3001)]
      GAME_SERVER_LIST = 3001
    }
  
}