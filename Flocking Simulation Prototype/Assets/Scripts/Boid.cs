using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Boid : MonoBehaviour
{
    private Vector3 m_Velocity;
    private Vector3 m_Acceleration;
    private Vector3 m_Forward;

    [SerializeField] private float m_Mass = 20.0f;
    [SerializeField] private float m_ArrivalDistance = 2.0f;
    [SerializeField] private float m_MaxSpeed = 0.2f;

    [SerializeField] private Transform m_MeshTransform;
    [SerializeField] private Transform m_Target;

    private void Start()
    {
        m_Velocity = Vector3.forward;
        m_Forward = m_Velocity;
    }

    private void FixedUpdate()
    {
        Seek(m_Target.position);
        PhysicsUpdate();
    }

    private void PhysicsUpdate()
    {
        // update velocity
        m_Velocity += m_Acceleration;
        if(m_Velocity.magnitude > m_MaxSpeed) {
            m_Velocity.Normalize();
            m_Velocity *= m_MaxSpeed;
        }

        // update forward
        m_Forward = Vector3.Normalize(m_Velocity);
        m_MeshTransform.LookAt(m_MeshTransform.position + m_Forward);

        // translate
        transform.Translate(m_Velocity);

        // reset acceleration
        m_Acceleration = Vector3.zero;
    }

    private void AddForce(Vector3 force)
    {
        m_Acceleration += force / m_Mass;
    }

    private void Steer(Vector3 desired)
    {
        AddForce(desired - m_Velocity);
    }

    private void Seek(Vector3 target)
    {
        Vector3 offset = target - transform.position;
        Vector3 direction = Vector3.Normalize(offset);
        float distance = offset.magnitude;

        float arrivalFactor = distance / m_ArrivalDistance;
        float speed = Mathf.Clamp01(arrivalFactor);

        Vector3 desired = speed * direction;
        Steer(desired);
    }
}
