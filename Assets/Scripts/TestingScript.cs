using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class TestingScript : MonoBehaviour
{
    public GameObject testLight;

    void Start()
    {
        if ( testLight != null )
        {
            testLight.SetActive( false );
        }
    }
}
