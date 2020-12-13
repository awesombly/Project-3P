using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Actor : MonoBehaviour
{
    [HideInInspector]
    public uint serial;
    [HideInInspector]
    public bool isLocal;
    [HideInInspector]
    public Vector3 syncVelocity;

    [HideInInspector]
    public Rigidbody rigidBody;

    protected virtual void Awake()
    {
        isLocal = true;
        rigidBody = GetComponent<Rigidbody>();
    }

    protected virtual void Update()
    {
        if ( !isLocal )
        {
            rigidBody.velocity = syncVelocity;
        }
    }
}
