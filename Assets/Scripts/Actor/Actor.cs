using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Actor : MonoBehaviour
{
    internal uint serial;
    internal bool isLocal;
    internal Vector3 syncVelocity;

    internal Rigidbody rigidBody;

    protected virtual void Awake()
    {
        isLocal = true; // 테스트용
        rigidBody = GetComponent<Rigidbody>();
    }

    protected virtual void FixedUpdate()
    {
        if ( !isLocal )
        {
            rigidBody.velocity = syncVelocity;
        }
    }
}
