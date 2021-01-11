using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.AddressableAssets;

public class VillageScene : SceneBase
{
    public AssetReference loadingScene;

    protected override void Awake()
    {
        if ( !ResourceManager.Instance.isInited )
        {
            ChangeSceneAsync( loadingScene );
            return;
        }
        
        base.Awake();
    }

    protected override void Start()
    {
        if ( !ResourceManager.Instance.isInited )
        {
            return;
        }

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
