using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class Flock : MonoBehaviour
{
    public static Flock s_Instance = null;

    private List<Boid> m_Boids;

    [SerializeField] private float m_Size = 10.0f;
    [SerializeField] private int m_SpawnCount = 10;
    [SerializeField] private GameObject m_BoidPrefab;
    [SerializeField] private GameObject m_EdgePrefab;

    [SerializeField] private Slider m_SeparationSlider;
    [SerializeField] private Slider m_AlignmentSlider;
    [SerializeField] private Slider m_CohesionSlider;

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
        InitBoids();
        InitBoundingBoxEdges();
    }

    private void OnDrawGizmos()
    {
        Gizmos.DrawWireCube(transform.position, new Vector3(m_Size, m_Size, m_Size));
    }

    private void InitBoids()
    {
        m_Boids = new List<Boid>();
        for (int i = 0; i < m_SpawnCount; i++)
        {
            Boid boid = Instantiate(m_BoidPrefab).GetComponent<Boid>();
            boid.transform.SetParent(transform);

            boid.transform.position = new Vector3(Random.Range(-m_Size / 2, m_Size / 2), Random.Range(-m_Size / 2, m_Size / 2), Random.Range(-m_Size / 2, m_Size / 2));

            boid.SetVelocity(Random.onUnitSphere.normalized);

            m_Boids.Add(boid);
        }
    }

    private void InitBoundingBoxEdges()
    {
        Vector3[,] cubePositions =
        {
            { new Vector3(-0.5f, -0.5f, -0.5f), new Vector3( 0.5f, -0.5f, -0.5f) },
            { new Vector3(-0.5f,  0.5f, -0.5f), new Vector3( 0.5f,  0.5f, -0.5f) },
            { new Vector3(-0.5f, -0.5f, -0.5f), new Vector3(-0.5f,  0.5f, -0.5f) },
            { new Vector3( 0.5f, -0.5f, -0.5f), new Vector3( 0.5f,  0.5f, -0.5f) },
            { new Vector3(-0.5f, -0.5f, -0.5f), new Vector3(-0.5f, -0.5f,  0.5f) },
            { new Vector3(-0.5f,  0.5f, -0.5f), new Vector3(-0.5f,  0.5f,  0.5f) },
            { new Vector3( 0.5f, -0.5f, -0.5f), new Vector3( 0.5f, -0.5f,  0.5f) },
            { new Vector3( 0.5f,  0.5f, -0.5f), new Vector3( 0.5f,  0.5f,  0.5f) },
            { new Vector3(-0.5f, -0.5f,  0.5f), new Vector3( 0.5f, -0.5f,  0.5f) },
            { new Vector3(-0.5f,  0.5f,  0.5f), new Vector3( 0.5f,  0.5f,  0.5f) },
            { new Vector3(-0.5f, -0.5f,  0.5f), new Vector3(-0.5f,  0.5f,  0.5f) },
            { new Vector3( 0.5f, -0.5f,  0.5f), new Vector3( 0.5f,  0.5f,  0.5f) }
        };

        for (int i = 0; i < cubePositions.GetLength(0); i++)
        {
            LineRenderer edge = Instantiate(m_EdgePrefab).GetComponent<LineRenderer>();
            Vector3[] edgePositions = { cubePositions[i, 0] * m_Size, cubePositions[i, 1] * m_Size };
            edge.transform.parent = transform;
            edge.SetPositions(edgePositions);
        }
    }

    public List<Boid> GetBoids()
    {
        return m_Boids;
    }

    public float GetSize()
    {
        return m_Size;
    }

    public float GetSeparationWeight()
    {
        return m_SeparationSlider.value;
    }

    public float GetAlignmentWeight()
    {
        return m_AlignmentSlider.value;
    }

    public float GetCohesionWeight()
    {
        return m_CohesionSlider.value;
    }
}
