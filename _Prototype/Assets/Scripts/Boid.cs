using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Boid : MonoBehaviour
{
    private Vector3 m_Velocity = Vector3.forward;
    private Vector3 m_Acceleration = Vector3.zero;
    private Vector3 m_Forward = Vector3.forward;

    [Header("Physics")]
    [SerializeField] private float m_Mass = 20.0f;
    [SerializeField] private float m_ArrivalDistance = 2.0f;
    [SerializeField] private float m_MaxSpeed = 0.2f;
    [SerializeField] private float m_MaxForce = 0.1f;

    [Header("Flock")]
    [SerializeField] private float m_SeparateDistance = 0.5f;
    [SerializeField] private float m_AlignDistance = 2.0f;
    [SerializeField] private float m_CohereDistance = 2.0f;

    [Header("Reference")]
    [SerializeField] private Transform m_MeshTransform;

    private void Start()
    {
        
    }

    private void FixedUpdate()
    {
        Separate();
        Align();
        Cohere();

        PhysicsUpdate();
        Mirror();
    }

    private void Mirror()
    {
        float radius = Flock.GetInstance().GetSize() * 0.5f;
        Vector3 localPosition = transform.position - Flock.GetInstance().transform.position;

        float x = localPosition.x;
        float y = localPosition.y;
        float z = localPosition.z;

        if (localPosition.x > radius)       x = -2.0f * radius + localPosition.x;
        else if (localPosition.x < -radius) x =  2.0f * radius - localPosition.x;
        if (localPosition.y > radius)       y = -2.0f * radius + localPosition.y;
        else if (localPosition.y < -radius) y =  2.0f * radius - localPosition.y;
        if (localPosition.z > radius)       z = -2.0f * radius + localPosition.z;
        else if (localPosition.z < -radius) z =  2.0f * radius - localPosition.z;

        transform.position = Flock.GetInstance().transform.position + new Vector3(x, y, z);
    }

    #region physics

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

    #endregion

    #region steering

    private void Steer(Vector3 desired)
    {
        Vector3 force = Vector3.ClampMagnitude(desired - m_Velocity, m_MaxForce);
        AddForce(desired - m_Velocity);
    }

    private void Seek(Vector3 target, float maxSpeed, float arrivalDistance)
    {
        Vector3 offset = target - transform.position;
        Vector3 direction = Vector3.Normalize(offset);
        float distance = offset.magnitude;

        Vector3 desired = maxSpeed * direction;
        if(distance < arrivalDistance) {
            desired *= distance / arrivalDistance;
        }

        Steer(desired);
    }

    private void Flee(Vector3 target, float maxSpeed)
    {
        Vector3 offset = transform.position - target;
        Vector3 direction = Vector3.Normalize(offset);

        Vector3 desired = maxSpeed * direction;

        Steer(desired);
    }

    #endregion

    #region flock

    void Separate()
    {
        List<Boid> boids = Flock.GetInstance().GetBoids();
        float weight = Flock.GetInstance().GetSeparationWeight();

        foreach(Boid boid in boids) {

            if (boid == this) continue;

            float distance = Vector3.Distance(transform.position, boid.transform.position);

            if(distance < m_SeparateDistance) {
                Vector3 desired = Vector3.Normalize(transform.position - boid.transform.position);
                desired *= Mathf.Clamp(m_SeparateDistance / distance, 0f, m_MaxSpeed * weight);
                Steer(desired);
            }

        }
    }

    void Align()
    {
        List<Boid> boids = Flock.GetInstance().GetBoids();
        float weight = Flock.GetInstance().GetAlignmentWeight();

        Vector3 averageForward = Vector3.zero;

        foreach (Boid boid in boids) {

            if (boid == this) continue;

            float distance = Vector3.Distance(transform.position, boid.transform.position);

            if(distance < m_AlignDistance) {
                averageForward += boid.GetForward();
            }
        }

        averageForward.Normalize();
        averageForward *= m_MaxSpeed * weight;

        Steer(averageForward);
    }

    void Cohere()
    {
        List<Boid> boids = Flock.GetInstance().GetBoids();
        float weight = Flock.GetInstance().GetCohesionWeight();

        Vector3 averagePosition = Vector3.zero;

        foreach (Boid boid in boids) {

            if (boid == this) continue;

            float distance = Vector3.Distance(transform.position, boid.transform.position);

            if (distance < m_CohereDistance) {
                averagePosition += boid.transform.position;
            }
        }

        if(boids.Count > 1) {
            averagePosition /= boids.Count - 1;
        }

        Seek(averagePosition, m_MaxSpeed * weight, m_ArrivalDistance);
    }

    #endregion

    #region getters_setters

    public void SetVelocity(Vector3 velocity)
    {
        m_Velocity = velocity;
        m_Forward = m_Velocity;
    }

    public Vector3 GetForward()
    {
        return m_Forward;
    }

    #endregion
}
