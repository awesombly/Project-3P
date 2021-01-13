using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[RequireComponent( typeof( AudioSource ) )]
public class AudioManager : Singleton<AudioManager>
{
    internal AudioData audioData;

    private AudioSource audioSource;

    private void Awake()
    {
        audioSource = GetComponent<AudioSource>();
    }

    public void PlayBgm()
    {
        if ( ReferenceEquals( audioData, null ) )
        {
            return;
        }

        AudioClip clip = audioData.GetRandomBgm();
        if ( ReferenceEquals( clip, null ) )
        {
            return;
        }

        audioSource.volume = audioData.bgmVolume;
        audioSource.clip = clip;
        audioSource.Play();
    }

    public void PlaySound( AudioClip _sound )
    {
        audioSource.PlayOneShot( _sound, audioData.seVolume );
    }

    public void PlaySound( List<AudioClip> _sounds )
    {
        if ( _sounds.Count <= 0 )
        {
            return;
        }

        PlaySound( _sounds[ Random.Range( 0, _sounds.Count ) ] );
    }
}
