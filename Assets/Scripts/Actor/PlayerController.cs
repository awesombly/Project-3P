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

    private Animator animator;

    private const float AnimationSmooth = 0.2f;
    private float inputVertical;
    private float inputHorizontal;
    private float inputMagnitude;

    protected override void Awake()
    {
        base.Awake();

        animator = GetComponentInChildren<Animator>();
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

    private void UpdateInputParameters()
    {
        float sprintingRate = ( isSprinting ? 1.5f : 1.0f );

        Vector3 relativeInput = transform.InverseTransformDirection( rigidBody.velocity );
        inputVertical = Mathf.Clamp( relativeInput.z, -1.0f, 1.0f ) * sprintingRate;
        inputHorizontal = Mathf.Clamp( relativeInput.x, -1.0f, 1.0f ) * sprintingRate;

        inputMagnitude = relativeInput.normalized.magnitude;
    }

    private void UpdateAnimatorParameters()
    {
        animator.SetBool( AnimatorParameters.IsGrounded, isGrounded );
        animator.SetBool( AnimatorParameters.IsStrafing, true );
        animator.SetBool( AnimatorParameters.IsSprinting, isSprinting );

        bool isStop = rigidBody.velocity.magnitude < float.Epsilon;
        animator.SetFloat( AnimatorParameters.InputHorizontal, isStop ? 0.0f : inputHorizontal, AnimationSmooth, Time.deltaTime );
        animator.SetFloat( AnimatorParameters.InputVertical, isStop ? 0.0f : inputVertical, AnimationSmooth, Time.deltaTime );
        animator.SetFloat( AnimatorParameters.InputMagnitude, isStop ? 0.0f : inputMagnitude, AnimationSmooth, Time.deltaTime );
    }

    private static class AnimatorParameters
    {
        public static int InputHorizontal = Animator.StringToHash( "InputHorizontal" );
        public static int InputVertical = Animator.StringToHash( "InputVertical" );
        public static int InputMagnitude = Animator.StringToHash( "InputMagnitude" );
        public static int GroundDistance = Animator.StringToHash( "GroundDistance" );
        public static int IsGrounded = Animator.StringToHash( "IsGrounded" );
        public static int IsStrafing = Animator.StringToHash( "IsStrafing" );
        public static int IsSprinting = Animator.StringToHash( "IsSprinting" );
    }
}
