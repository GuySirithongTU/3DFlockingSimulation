using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Rendering;

public class Flock : MonoBehaviour
{
    public static Flock s_Instance = null;

    private List<Boid> m_Boids;

    [SerializeField] private float m_Size = 10.0f;
    [SerializeField] private int m_SpawnCount = 10;
    [SerializeField] private GameObject m_BoidPrefab;

    public static Flock GetInstance()
    {
        return s_Instance;
    }

    private void Awake()
    {
        if(s_Instance == null) {
            s_Instance = this;
        } else {
            Debug.LogError("duplicate flock");
        }
    }

    private void Start()
    {
        m_Boids = new List<Boid>();

        for(int i = 0; i < m_SpawnCount; i++)
        {
            Boid boid = Instantiate(m_BoidPrefab).GetComponent<Boid>();
            boid.transform.SetParent(transform);

            boid.transform.position = new Vector3(Random.Range(-m_Size / 2, m_Size / 2), Random.Range(-m_Size / 2, m_Size / 2), Random.Range(-m_Size / 2, m_Size / 2));

            boid.SetVelocity(Random.onUnitSphere.normalized);

            m_Boids.Add(boid);
        }
    }

    private void OnDrawGizmos()
    {
        Gizmos.DrawWireCube(transform.position, new Vector3(m_Size, m_Size, m_Size));
    }

    public List<Boid> GetBoids()
    {
        return m_Boids;
    }

    public float GetSize()
    {
        return m_Size;
    }
}
