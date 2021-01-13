using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.AddressableAssets;

public class LoadingScene : SceneBase
{
    public UnityEngine.UI.Slider loadingBar;

    protected override void Awake()
    {
        base.Awake();

        ResourceManager.Instance.OnChangeLoadedRatio += OnChangeLoadedRatio;
        ResourceManager.Instance.Init();
    }

    protected override void Start()
    {
        base.Start();
    }

    protected override void OnDestroy()
    {
        base.OnDestroy();
        ResourceManager.Instance.OnChangeLoadedRatio -= OnChangeLoadedRatio;
    }

    protected override void OnConnect()
    {
        Debug.LogWarning( "Invalid scene connect." );
    }

    protected override void OnBindProtocols()
    {
    }

    private void OnChangeLoadedRatio( float _loadedRatio )
    {
        loadingBar.value = _loadedRatio;

        if ( _loadedRatio >= 1.0f )
        {
            SceneManager.LoadScene( nextScene );
        }
    }
}
