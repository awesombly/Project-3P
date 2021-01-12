using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.AddressableAssets;

public class VillageScene : SceneBase
{
    protected override void Awake()
    {
        base.Awake();
    }

    protected override void Start()
    {
        base.Start();

        if ( ObjectManager.Instance.LocalPlayer == null )
        {
            Player player = FindObjectOfType<Player>();
            if ( player != null && player.isLocal )
            {
                ObjectManager.Instance.LocalPlayer = player;
            }
        }

        StartCoroutine( Network.Instance.RunNetwork() );
    }

    protected override void OnDestroy()
    {
        base.OnDestroy();
    }

    protected override void OnConnect()
    {
        base.OnConnect();
    }

    protected override void OnBindProtocols()
    {
        base.OnBindProtocols();
    }
}
