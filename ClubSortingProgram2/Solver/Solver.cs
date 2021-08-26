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
                MainScreen.Instance.AlertError("Could not add person \"" + s.Name + "\" because assignments have been created.");
                return;
            }
            try
            {
                _people.Add(s.Name, s);
            }
            catch (ArgumentException)
            {
                MainScreen.Instance.AlertError("Found duplicate name: \"" + s.Name + "\".");
            }
        }

        public void AddGroup(Group c)
        {
            if (_solved)
            {
                MainScreen.Instance.AlertError("Could not add club \"" + c.Name + "\" because assignments have been created.");
                return;
            }
            try
            {
                _groups.Add(c.Name, c);
            }
            catch (ArgumentException)
            {
                MainScreen.Instance.AlertError("Found duplicate group name: \"" + c.Name + "\".");
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
                MainScreen.Instance.AlertError("Could not find optimal solution.");
                return;
            }

            //TODO Find answers and update objects
            _solved = true;
        }

        public Group[] GetGroups(string name)
        {
            if (!_solved)
            {
                MainScreen.Instance.AlertError("Group assignments have not been created yet.");
                return null;
            }
            if (!_people.ContainsKey(name))
            {
                MainScreen.Instance.AlertError("Person \"" + name + "\" does not exist.");
                return null;
            }
            return _people[name].Assignments;
        }

        public Person[] GetMembers(string name)
        {
            if (!_solved)
            {
                MainScreen.Instance.AlertError("Group assignments have not been created yet.");
                return null;
            }
            if (!_groups.ContainsKey(name))
            {
                MainScreen.Instance.AlertError("Group \"" + name + "\" does not exist.");
                return null;
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
                _assignmentArcMap.Add(name, new int[Settings.MaxRequests + 1]);
                int numr = _people[name].Requests[_section].Count;

                if (numr != Settings.MaxRequests)
                {
                    MainScreen.Instance.AlertError("Person " + name + " has " + numr.ToString() + " requests, expected " + Settings.MaxRequests.ToString() + ".");
                    return false;
                }

                for (int i = 0; i < Settings.MaxRequests; i++)
                {
                    Group request = _people[name].Requests[_section][i];
                    int arc = mcf.AddArcWithCapacityAndUnitCost(_peopleNodeMap[name], _groupNodeMap[request.Name], 1, Settings.RequestWeights[i]);
                    _assignmentArcMap[name][i] = arc;
                }
                int arc2 = mcf.AddArcWithCapacityAndUnitCost(_peopleNodeMap[name], _groupNodeMap[Settings.DefaultGroupName], 1, Settings.RequestWeights[Settings.MaxRequests]);
                _assignmentArcMap[name][Settings.MaxRequests] = arc2;
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