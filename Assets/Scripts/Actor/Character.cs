using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Character : Actor
{
    private struct SyncMovement
    {
        public const float NeedInterval = 0.1f;
        public Vector3 PrevVelocity;
        public float PrevSqrMagnitude;
    }
    private SyncMovement syncMovement;

    internal Animator animator;
    protected CapsuleCollider capsule;
    protected float originCapsuleHeight;

    protected const float AnimationDampTime = 0.2f;
    protected float inputVertical;
    protected float inputHorizontal;
    protected float inputMagnitude;

    protected override void Awake()
    {
        base.Awake();

        animator = GetComponentInChildren<Animator>();

        capsule = GetComponent<CapsuleCollider>();
        originCapsuleHeight = capsule.height;
    }

    protected override void FixedUpdate()
    {
        base.FixedUpdate();

        UpdateInputParameters();
        UpdateAnimatorParameters();
        
        if ( !isLocal )
        {
            return;
        }

        float velocityInterval = Vector3.Distance( rigidBody.velocity, syncMovement.PrevVelocity );
        if ( velocityInterval > SyncMovement.NeedInterval )
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

        /// TODO : 정지중일때 방향전환시 동기화
    }

    protected virtual void UpdateInputParameters()
    {
        inputVertical = Mathf.Clamp( localVelocity.z, -1.0f, 1.0f );
        inputHorizontal = Mathf.Clamp( localVelocity.x, -1.0f, 1.0f );

        inputMagnitude = localVelocity.normalized.magnitude;
    }

    protected virtual void UpdateAnimatorParameters()
    {
        if ( ReferenceEquals( animator, null ) )
        {
            return;
        }

        bool isStop = rigidBody.velocity.sqrMagnitude < float.Epsilon;
        animator.SetFloat( AnimatorParameters.InputHorizontal, isStop ? 0.0f : inputHorizontal, AnimationDampTime, Time.deltaTime );
        animator.SetFloat( AnimatorParameters.InputVertical, isStop ? 0.0f : inputVertical, AnimationDampTime, Time.deltaTime );
        animator.SetFloat( AnimatorParameters.InputMagnitude, isStop ? 0.0f : inputMagnitude, AnimationDampTime, Time.deltaTime );
        animator.SetFloat( AnimatorParameters.VelocityY, rigidBody.velocity.y );
    }

    protected static class AnimatorParameters
    {
        public static int InputHorizontal = Animator.StringToHash( "InputHorizontal" );
        public static int InputVertical = Animator.StringToHash( "InputVertical" );
        public static int InputMagnitude = Animator.StringToHash( "InputMagnitude" );
        public static int VelocityY = Animator.StringToHash( "VelocityY" );
        public static int GroundDistance = Animator.StringToHash( "GroundDistance" );
        public static int IsGrounded = Animator.StringToHash( "IsGrounded" );
        public static int IsStrafing = Animator.StringToHash( "IsStrafing" );
        public static int IsSprinting = Animator.StringToHash( "IsSprinting" );
        public static int IsCrouching = Animator.StringToHash( "IsCrouching" );

        /* AI */
        public static int AIState = Animator.StringToHash( "State" );
    }
}
