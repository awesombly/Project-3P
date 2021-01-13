using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.AddressableAssets;

[CreateAssetMenu( fileName = "AudioData", menuName = "Scriptable Object/AudioData" )]
public class AudioData : ScriptableObject
{
    public float bgmVolume = 0.5f;
    public float seVolume = 0.5f;

    public List<AssetReferenceAudioClip> bgms = new List<AssetReferenceAudioClip>();

    public AudioClip GetRandomBgm()
    {
        if ( bgms.Count <= 0 )
        {
            return null;
        }

        AssetReferenceAudioClip asset = bgms[ Random.Range( 0, bgms.Count ) ];

        return ResourceManager.Instance.GetAsset<AudioClip>( asset ); ;
    }
}
