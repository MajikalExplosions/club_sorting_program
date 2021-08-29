using Google.OrTools.Graph;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ClubSortingProgram2.Solver
{
    public class Solver
    {
        public static readonly SolverSettings Settings;
        private readonly SortedList<string, Group> _groups;
        private readonly SortedList<string, Person> _people;

        private Dictionary<string, int[]> _assignmentArcMap;
        private Dictionary<string, int> _peopleNodeMap;
        private Dictionary<string, int> _groupNodeMap;

        private const int SINK = 0;

        private bool _solved;
        private int _section;

        public Solver(int section)
        {
            _section = section;
            _groups = new SortedList<string, Group>();
            _people = new SortedList<string, Person>();

            _groups.Add(Settings.DefaultGroupName, new Group(Settings.DefaultGroupName, true));
        }

        public void AddPerson(Person s)
        {
            if (_solved)
            {
                throw new SolverException(300, string.Format("Could not add person \"{0}\" because assignments have already been created.", s.Name));
            }
            try
            {
                _people.Add(s.Name, s);
            }
            catch (ArgumentException)
            {
                throw new SolverException(301, string.Format("Found duplicate person name \"{0}\".", s.Name));
            }
        }

        public void AddGroup(Group c)
        {
            if (_solved)
            {
                throw new SolverException(302, string.Format("Could not add group \"{0}\" because assignments have already been created.", c.Name));
            }
            try
            {
                _groups.Add(c.Name, c);
            }
            catch (ArgumentException)
            {
                throw new SolverException(303, string.Format("Found duplicate group name \"{0}\".", c.Name));
            }
        }

        public void Solve()
        {
            if (_solved) return;
            //TODO Finish this section (see notes)

            MinCostFlow minCostFlow = new MinCostFlow();
            _populateNodeMaps();
            _createGroupCapacityArcs(minCostFlow);
            _createAssignmentArcs(minCostFlow);

            //Set supplies
            minCostFlow.SetNodeSupply(SINK, _people.Count);
            foreach (string name in _peopleNodeMap.Keys)
            {
                minCostFlow.SetNodeSupply(_peopleNodeMap[name], 1);
            }

            MinCostFlowBase.Status status = minCostFlow.SolveMaxFlowWithMinCost();
            if (status != MinCostFlowBase.Status.OPTIMAL)
            {
                throw new SolverException(304, string.Format("Coud not find optimal solution."));
            }

            //TODO Find answers and update objects
            _solved = true;
        }

        public Group[] GetGroups(string name)
        {
            if (!_solved)
            {
                throw new SolverException(305, string.Format("Assignments have not been created yet."));
            }
            if (!_people.ContainsKey(name))
            {
                throw new SolverException(306, string.Format("Person \"{0}\" does not exist", name));
            }
            return _people[name].Assignments;
        }

        public Person[] GetMembers(string name)
        {
            if (!_solved)
            {
                throw new SolverException(307, string.Format("Assignments have not been created yet."));
            }
            if (!_groups.ContainsKey(name))
            {
                throw new SolverException(308, string.Format("Group \"{0}\" does not exist", name));
            }
            return _groups[name].Members.ToArray();
        }

        private void _createGroupCapacityArcs(MinCostFlow mcf)
        {
            foreach (string name in _groupNodeMap.Keys)
            {
                mcf.AddArcWithCapacityAndUnitCost(_groupNodeMap[name], SINK, _groups[name].MaximumCapacity, 0);
            }
        }

        private bool _createAssignmentArcs(MinCostFlow mcf)
        {
            _assignmentArcMap = new Dictionary<string, int[]>();
            foreach (string name in _people.Keys)
            {
                //Go through choices for each person
                int numReq = _people[name].Requests[_section].Count;
                _assignmentArcMap.Add(name, new int[numReq + 1]);

                if (numReq > Settings.MaxRequests)
                {
                    throw new SolverException(309, string.Format("Person {0} has too many requests ({1}).", name, numReq));
                }

                for (int i = 0; i < numReq; i++)
                {
                    Group request = _people[name].Requests[_section][i];
                    int arc = mcf.AddArcWithCapacityAndUnitCost(_peopleNodeMap[name], _groupNodeMap[request.Name], 1, Settings.RequestWeights[i]);
                    _assignmentArcMap[name][i] = arc;
                }
                int arc2 = mcf.AddArcWithCapacityAndUnitCost(_peopleNodeMap[name], _groupNodeMap[Settings.DefaultGroupName], 1, Settings.RequestWeights[numReq]);
                _assignmentArcMap[name][numReq] = arc2;
            }
            return true;
        }

        private void _populateNodeMaps()
        {
            _peopleNodeMap = new Dictionary<string, int>();
            _groupNodeMap = new Dictionary<string, int>();

            int index = SINK + 1;
            foreach (string name in _people.Keys)
            {
                _peopleNodeMap[name] = index++;
            }
            foreach (string name in _groups.Keys)
            {
                _groupNodeMap[name] = index++;
            }
            _groupNodeMap[Settings.DefaultGroupName] = index;
        }

        public struct SolverSettings
        {
            public readonly int MaximumCapacity;
            public readonly int Sections;
            public readonly string DefaultGroupName;
            public readonly bool RequestsHaveOrder;
            public readonly int MaxRequests;
            public readonly int[] RequestWeights;

            public SolverSettings(int mc, int s, string dgn, bool rho, int mr, int requestWeightMultiplier)
            {
                MaximumCapacity = mc;
                Sections = s;
                DefaultGroupName = dgn;
                RequestsHaveOrder = rho;
                MaxRequests = mr;
                RequestWeights = new int[MaxRequests + 1];
                RequestWeights[0] = 1;
                if (RequestsHaveOrder)
                {
                    for (int i = 1; i < RequestWeights.Length; i++)
                    {
                        RequestWeights[i] = RequestWeights[i - 1] * requestWeightMultiplier;
                    }
                }
                else
                {
                    for (int i = 1; i < RequestWeights.Length - 1; i++)
                    {
                        RequestWeights[i] = RequestWeights[i - 1];
                    }
                    RequestWeights[RequestWeights.Length - 1] = (int)Math.Pow(requestWeightMultiplier, MaxRequests);
                }
            }
        }
    }
}