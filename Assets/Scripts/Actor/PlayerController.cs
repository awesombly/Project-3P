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

    private FirstPersonAIO firstPerson;
    private Animator animator;

    private const float AnimationSmooth = 0.2f;
    private float inputVertical;
    private float inputHorizontal;
    private float inputMagnitude;

    protected override void Awake()
    {
        base.Awake();

        firstPerson = GetComponent<FirstPersonAIO>();
        animator = GetComponentInChildren<Animator>();
    }

    protected void Update()
    {
        UpdateInputParameters();

        //animator.SetFloat( AnimatorParameters.GroundDistance, groundDistance );
        animator.SetBool( AnimatorParameters.IsGrounded, firstPerson.IsGrounded );
        animator.SetBool( AnimatorParameters.IsStrafing, true );
        animator.SetBool( AnimatorParameters.IsSprinting, firstPerson.isSprinting );

        bool isStop = rigidBody.velocity.magnitude < float.Epsilon;
        animator.SetFloat( AnimatorParameters.InputHorizontal, isStop ? 0.0f : inputHorizontal, AnimationSmooth, Time.deltaTime );
        animator.SetFloat( AnimatorParameters.InputVertical, isStop ? 0.0f : inputVertical, AnimationSmooth, Time.deltaTime );
        animator.SetFloat( AnimatorParameters.InputMagnitude, isStop ? 0.0f : inputMagnitude, AnimationSmooth, Time.deltaTime );
    }

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

    public virtual void UpdateInputParameters()
    {
        float sprintingRate = ( firstPerson.isSprinting ? 1.5f : 1.0f );

        Vector3 relativeInput = transform.InverseTransformDirection( rigidBody.velocity );
        inputVertical = Mathf.Clamp( relativeInput.z, -1.0f, 1.0f ) * sprintingRate;
        inputHorizontal = Mathf.Clamp( relativeInput.x, -1.0f, 1.0f ) * sprintingRate;

        inputMagnitude = relativeInput.normalized.magnitude;
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
