using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerController : Actor
{
    private struct SyncMovement
    {
        public const float NeedInterval = 0.1f;
        public Vector3 PrevVelocity;
        public float PrevSqrMagnitude;
    }
    private SyncMovement syncMovement;

    protected override void FixedUpdate()
    {
        base.FixedUpdate();

        if ( !isLocal )
        {
            return;
        }

        float velocityInterval = Vector3.Distance( rigidBody.velocity, syncMovement.PrevVelocity );
        bool isStopped = ( rigidBody.velocity.sqrMagnitude < syncMovement.PrevSqrMagnitude && rigidBody.velocity.sqrMagnitude < float.Epsilon );
        if ( isStopped || velocityInterval > SyncMovement.NeedInterval )
        {
            Protocol.Both.SyncInterpolation protocol;
            protocol.Actor.Serial = serial;
            protocol.Actor.Position = transform.position;
            protocol.Actor.Rotation = transform.rotation;
            protocol.Velocity = rigidBody.velocity;

            Network.Instance.Send( protocol );
        }
        syncMovement.PrevVelocity = rigidBody.velocity;
        syncMovement.PrevSqrMagnitude = rigidBody.velocity.sqrMagnitude;
    }
}
