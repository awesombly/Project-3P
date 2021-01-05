using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Actor : MonoBehaviour
{
    internal uint serial;
    internal bool isLocal = true;
    internal Vector3 syncVelocity;
    internal Vector3 localVelocity;

    internal Rigidbody rigidBody;

    protected virtual void Awake()
    {
        rigidBody = GetComponent<Rigidbody>();
    }

    protected virtual void FixedUpdate()
    {
        if ( !isLocal )
        {
            rigidBody.velocity = syncVelocity;
        }

        localVelocity = transform.InverseTransformDirection( rigidBody.velocity );
    }
}
