#include "Cpu.hpp"
#include <rssd/Core.h>

using namespace std;
using namespace rssd;
using namespace rssd::relax;
using namespace rssd::relax::cpu;

///
/// Functions
///

int rssd::relax::cpu::cpu_main(const size_t num_markers)
{
  // Create singletons
  new TimerManager();

  // Create high-resolution timer
  PosixTimer *timer = dynamic_cast<PosixTimer*>(TimerManager::getPointer()->create(
      PosixTimerFactory::getPointer()->getType()));

  // Configure random number generator
  rng::MAX_RANGE = 1000.0f;
  rng::MIN_RANGE = -1000.0f;

  // Local vars
  marker_v
    objects,
    labels,
    reference_labels;

  // Generate test data
  BEGIN_TRACE("GENERATE DATA", timer);
  {
    if (!generateData(
      objects,
      labels,
      reference_labels,
      num_markers))
      return -1;
  }
  END_TRACE("GENERATE DATA", timer);

  // Calculate inter-label distances
  BEGIN_TRACE("CALCULATE LABEL DISTANCES", timer);
  {
    marker_distance_m label_distances;
    calculateInterLabelDistances(
      labels,
      label_distances);
  }
  END_TRACE("CALCULATE LABEL DISTANCES", timer);

#if 0
  // Calculate inter-object distances
  BEGIN_TRACE("CALCULATE OBJECT DISTANCES", timer);
  {
    marker_distance_m object_distances;
    calculateInterObjectDistances(
      objects,
      object_distances);
  }
  END_TRACE("CALCULATE OBJECT DISTANCES", timer);

  // Calculate inter-reference label distances
  BEGIN_TRACE("CALCULATE REFERENCE LABEL DISTANCES", timer);
  {
    marker_distance_m reference_label_distances;
    calculateInterLabelDistances(
      reference_labels,
      reference_label_distances);
  }
  END_TRACE("CALCULATE REFERENCE LABEL DISTANCES", timer);

  // Parse unique object-label/object-label pairs
  /// @note This is done to flatten the main loop.
  BEGIN_TRACE("GENERATE COMPATIBILITY PAIRS", timer);
  {
    compatibility_s compatibilities;
    getUniqueCompatabilityPairs(
      objects,
      labels,
      compatibilities);
  }
  END_TRACE("GENERATE COMPATIBILITY PAIRS", timer);
#endif

  // Destroy singletons
  delete TimerManager::getPointer();
  return 0;
}

void rssd::relax::cpu::calculateInterObjectDistances(
  const rssd::relax::marker_v &objects,
  rssd::relax::marker_distance_m &distances)
{
  // Local vars
  marker_pair_t pair;
  const marker_t *this_object = NULL;
  const marker_t *other_object = NULL;

  for (size_t o1 = 0; o1 < objects.size(); o1++)
  {
    for (size_t o2 = 0; o2 < objects.size(); o2++)
    {
      // Skip self-pairings
      this_object = &objects[o1];
      other_object = &objects[o2];
      if (*this_object == *other_object)
        continue;
        
      // Calculate distance b/w objects
      pair.set(*this_object, *other_object); 
      if (distances.find(pair) == distances.end())
        distances[pair] = this_object->distance(*other_object);
#if 0
      o_dist[o1][o2] = 
        sqrt(	(o_posXYZ[o1][0] - o_posXYZ[o2][0])*(o_posXYZ[o1][0] - o_posXYZ[o2][0]) +
            (o_posXYZ[o1][1] - o_posXYZ[o2][1])*(o_posXYZ[o1][1] - o_posXYZ[o2][1]) +
            (o_posXYZ[o1][2] - o_posXYZ[o2][2])*(o_posXYZ[o1][2] - o_posXYZ[o2][2]));
#endif
    }
  }
}

void rssd::relax::cpu::calculateInterLabelDistances(
  const rssd::relax::marker_v &labels,
  rssd::relax::marker_distance_m &distances)
{
  marker_pair_t pair;
  const marker_t *this_label = NULL;
  const marker_t *other_label = NULL;

	for (size_t l1 = 0; l1 < labels.size()-1; ++l1) 
  {
		for (size_t l2 = 0; l2 < labels.size()-1; ++l2) 
    {
      // Skip self-pairings
      this_label = &labels[l1];
      other_label = &labels[l2];
      if (*this_label == *other_label)
        continue;

      // Calculate distance b/w labels
      pair.set(*this_label, *other_label); 
      if (distances.find(pair) == distances.end())
        distances[pair] = this_label->distance(*other_label);
#if 0
			distances[pair] = std::sqrt(	
        (p_posXYZ[l1][0] - p_posXYZ[l2][0])*(p_posXYZ[l1][0] - p_posXYZ[l2][0]) +
        (p_posXYZ[l1][1] - p_posXYZ[l2][1])*(p_posXYZ[l1][1] - p_posXYZ[l2][1]) +
        (p_posXYZ[l1][2] - p_posXYZ[l2][2])*(p_posXYZ[l1][2] - p_posXYZ[l2][2]));
#endif
    } // end for(...)
  } // end for(...)
} // end rssd::calculateInterLabelDistances(...)

size_t rssd::relax::cpu::getUniqueCompatabilityPairs(
  const rssd::relax::marker_v &objects,
  const rssd::relax::marker_v &labels,
  rssd::relax::compatibility_s &compatibilities)
{
  // Local vars
  compatibility_t compatibility;
  marker_pair_t
    this_pair,
    other_pair;
  size_t
    loop_count = 0,
    dupe_count = 0;

  for (size_t i=0; i<objects.size()-1; ++i)
  {
    /// @note This -1 is b/c the noise label is a "unique label".
    for (size_t j=0; j<labels.size()-1; ++j)
    {
      for (size_t k=i+1; k<objects.size(); ++k)
      {
        /// @note This -1 is b/c the noise label is a "unique label".
        for (size_t l=0; l<labels.size()-1; ++l)
        {
          ++loop_count;

          // Configure current marker pair
          this_pair.set(objects[i], labels[j]), 
          other_pair.set(objects[k], labels[l]);

          // Skip self-pairings
          if (j == l)
            continue;

#if 0
          if (this_pair == other_pair)
          {
            std::cout << "=========================" << std::endl;
            std::cout << "Index = {" << i << ", " << j << ", " << k << ", " << l << "}\n"
              << "-------------------------\n"
              << this_pair.first_marker
              << this_pair.second_marker
              << other_pair.first_marker
              << other_pair.second_marker
              << std::endl << std::endl;
            ++dupe_count;
            continue;
          }
#endif

          // Assign 'marker pair' to temp compatibility object
          compatibility.set(this_pair, other_pair);

          // Insert compatibility object for the given 'marker pairs' if one is not already present
          //if (compatibilities.find(compatibility) == compatibilities.end())
            compatibilities.insert(compatibility);
        }
      }
    }
  }
  std::cout << "Loop count: " << loop_count << std::endl;
  std::cout << "Duplicate count: " << dupe_count << std::endl;
  std::cout << "Map size: " << compatibilities.size() << std::endl;
  return compatibilities.size();
} // end rssd::getUniqueCompatabilityPairs(...)

#if 0
void calculateDegreesOfFreedom(int* generationGapPtr, int l_numPts, int* labToMarkerID, bool is_left_handed)
{
#if display
  if (is_left_handed)
    cout << "Subject is left handed" << endl;
  else
    cout << "Subject is right handed" << endl;
#endif

#if !TEST
  int (*generationGap)[l_numPts-1] = (int (*)[l_numPts-1]) generationGapPtr;
#endif
  bool debug = false;

  // New approach to degreesOfFreedom calculation. There are "cross-points" on the body, aka joints.
  // If in travelling from one marker to another, you cross a cross-point, then degreesOfFreedom++.
  // Now we have to define the body: limbs and crosspoints and the relationships to these of various
  // markers and between markers. Each marker will lie on a limb. The limbs will be: torsoInterior (0),
  // head (1), right arm (2), left arm (3), right leg (4), and left leg (5). Each marker can potentially
  // have a cross-point underneath it, a cross-point on the torso-side, and a cross-point on the
  // extremity-side. The cross-points have limb ID's and are ordered within each limb, from closest to
  // torso (0), to furthest from torso (k). Then the markers are defined in terms of its position relative
  // to 1 or 2 cross-points. Its definition is either (a) on a cross-point, (b) outside of the cross-point
  // that is the furthest-from-torso cross point for some limb, (c) in between two cross-points, or (d)
  // on the interior/torso. For two markers on a given limb, you travel from the position of the
  // furthest-from-torso marker up the limb towards the next cross point, until you've reached the
  // closer-to-torso marker. The total number of cross-points you've crossed is your degreesOfFreedom. For
  // two markers on two different limbs, each one calculates the total number of cross-points it must cross
  // until it gets to the torsoInterior. The sum of these is the degreesOfFreedom.

  int myBodyID[36];
  int numBods = 15;
  // myBodyID: I simplify things into torso (0), head (1), shoulders (2 & 3), elbows (4 & 5), wrists (6 & 7), hips (8 & 9),
  // knees (10 & 11), ankles (12 & 13), feet (14 & 15), rifle (16 (later we'll do 17 for slide)). When there are paired members, we'll have
  // right side body parts be even numbered, and left side be odd.
  myBodyID[0] = 1;
  myBodyID[1] = 1;
  myBodyID[2] = 1;
  myBodyID[3] = 1;
  myBodyID[4] = 2;
  myBodyID[5] = 3;
  myBodyID[6] = 0;
  myBodyID[7] = 0;
  myBodyID[8] = 0;
  myBodyID[9] = 0;
  myBodyID[10] = 0;
  myBodyID[11] = 0;
  myBodyID[12] = 8;
  myBodyID[13] = 9;
  myBodyID[14] = 0;
  myBodyID[15] = 0;
  myBodyID[16] = 10;
  myBodyID[17] = 10;
  myBodyID[18] = 11;
  myBodyID[19] = 11;
  myBodyID[20] = 14;
  myBodyID[21] = 12;
  myBodyID[22] = 14;
  myBodyID[23] = 15;
  myBodyID[24] = 13;
  myBodyID[25] = 15;
  myBodyID[26] = 4;
  myBodyID[27] = 5;
  myBodyID[28] = 6;
  myBodyID[29] = 7;
  myBodyID[30] = 6;
  myBodyID[31] = 7;
  myBodyID[32] = 16;
  myBodyID[33] = 16;
  myBodyID[34] = 16;
  myBodyID[35] = 16;


  // The limb that each rigid body resides on. For our purposes, the head is considered a limb, while the torso
  // has value of 0, since it is the root and not a limb. Torso (0), head (1), right arm (2), left arm (3),
  // right leg (4), left leg (5). myBodyID is the index, and limbID is the value.
  int limbID[numBods];
  limbID[0] = 0;
  limbID[1] = 1;
  limbID[2] = 2;
  limbID[3] = 3;
  limbID[4] = 2;
  limbID[5] = 3;
  limbID[6] = 2;
  limbID[7] = 3;
  limbID[8] = 4;
  limbID[9] = 5;
  limbID[10] = 4;
  limbID[11] = 5;
  limbID[12] = 4;
  limbID[13] = 5;
  limbID[14] = 4;
  limbID[15] = 5;
  if (is_left_handed)
    limbID[16] = 3;
  else
    limbID[16] = 2;

  // Each body on a limb resides a certain number of units away from the root-joint of the limb. For example, the right shoulder
  // body is at the 0th spot of the right arm limb, since it is on the root joint. The right elbow is at spot # 2 on the right arm,
  // with a hypothetical marker between the right shoulder and the right elbow taking spot # 1 (neighboring joints are 2 spots apart,
  // whereas any markers in between the joints would take the odd spots). Units from the root-joint of the limb is the value, and
  // myBodyID is the index.
  int spotsAway[numBods];

  spotsAway[0] = 0;
  spotsAway[1] = 1;
  spotsAway[2] = 0;
  spotsAway[3] = 0;
  spotsAway[4] = 2;
  spotsAway[5] = 2;
  spotsAway[6] = 4;
  spotsAway[7] = 4;
  spotsAway[8] = 0;
  spotsAway[9] = 0;
  spotsAway[10] = 2;
  spotsAway[11] = 2;
  spotsAway[12] = 4;
  spotsAway[13] = 4;
  spotsAway[14] = 5;
  spotsAway[15] = 5;
  spotsAway[16] = 7;


  // Now, we find the "generational difference" b/w different label markers. If a label marker is on the same rigid body
  // as an otherLabel, the generational difference b/w the two is 0. Else, the generational difference is the sum of the
  // number of cross-points (or joints) you have to *cross* when travelling from one marker to the other. If a marker is
  // *on* a joint, this joint does not count in the sum of total crossed.

  for (int lab1=0; lab1<l_numPts-1; lab1++)
  {
    generationGap[lab1][lab1] = 0;
    int mar1 = labToMarkerID[lab1];
    if (debug) cout << "Lab 1 = " << lab1+1 << " . labToMarkerID[" << lab1+1 << "] = " << labToMarkerID[lab1]+1 <<
      " = mar1 = " << mar1+1 << " . myBodyID[" << mar1+1 << "] = " << myBodyID[mar1] <<
      ". limbID[" << myBodyID[mar1] << "] = " << limbID[myBodyID[mar1]] << endl;
    for (int lab2=lab1+1; lab2<l_numPts-1; lab2++) {
      int mar2 = labToMarkerID[lab2];
      if (myBodyID[mar1]==myBodyID[mar2]) {
        generationGap[lab1][lab2] = 0;
        generationGap[lab2][lab1] = 0;
        continue;
      }

      if (limbID[myBodyID[mar1]]==limbID[myBodyID[mar2]]) {
        int outerMar = mar2;
        int innerMar = mar1;
        if (spotsAway[myBodyID[mar1]] > spotsAway[myBodyID[mar2]]) {
          outerMar = mar1;
          innerMar = mar2;
        }

        int outerRoundedOut = spotsAway[myBodyID[outerMar]];
        int rem = outerRoundedOut % 2;
        if (rem != 0) outerRoundedOut += 1;

        int innerRoundedOut = spotsAway[myBodyID[innerMar]];
        rem = innerRoundedOut % 2;
        if (rem != 0) innerRoundedOut -= 1;

        int jointDiff = outerRoundedOut - innerRoundedOut;
        jointDiff /= 2;
        jointDiff -= 1;
        generationGap[lab1][lab2] = jointDiff;
        generationGap[lab2][lab1] = jointDiff;
      }
      else {
        int jointDiff1 = 0;
        int jointDiff2 = 0;
        int spotsRoundedOut = 0;

        if (spotsAway[myBodyID[mar1]] != 0) {
          if ((spotsAway[myBodyID[mar1]] % 2) != 0)
            spotsRoundedOut = spotsAway[myBodyID[mar1]] + 1;
          else
            spotsRoundedOut = spotsAway[myBodyID[mar1]];
          jointDiff1 = spotsRoundedOut/2;
        }
        if (spotsAway[myBodyID[mar2]] != 0) {
          if ((spotsAway[myBodyID[mar2]] % 2) != 0)
            spotsRoundedOut = spotsAway[myBodyID[mar2]] + 1;
          else
            spotsRoundedOut = spotsAway[myBodyID[mar2]];
          jointDiff2 = spotsRoundedOut/2;
        }
        generationGap[lab1][lab2] = jointDiff1 + jointDiff2;
        generationGap[lab2][lab1] = jointDiff1 + jointDiff2;
      }
    }
  }
}

/// @brief Find the 'generation gap' (degrees-of-freedom b/w markers).
/// @note A value of -1 indicates a NULL relationship b/w two different markers, e.g. if each is from a different subject.
void rssd::calculateGenerationGaps()
{
  int generationGap[l_numPts-1][l_numPts-1];
  BtDArray<BtDArray<int> > generationGap_i(problemCounts[i]);
  const int problemCounts[numSubjects] = {36, 36};

  for (int lab1=0; lab1<l_numPts-1; lab1++)
  {
    for (int lab2=0; lab2<l_numPts-1; lab2++)
    {
      generationGap[lab1][lab2] = -1;
    }
  }

  /// @note Each subject can have its 'generation gap' calculated separately,
  /// then the global collection of 'generation gaps' is populated.
  /// @note 'Generation gaps' b/w markers for a subject are initialized to 0.
  for (int i=0; i<numSubjects; i++)
  {
    int generationGap[problemCounts[i]][problemCounts[i]];
    for (int lab1=0; lab1<problemCounts[i]; lab1++)
    {
      for (int lab2=0; lab2<problemCounts[i]; lab2++)
      {
        generationGap_i[lab1][lab2] = 0;
      }
    }

    int labToMarkerID_i[problemCounts[i]];
    for (int lab=0; lab<problemCounts[i]; lab++)
    {
      labToMarkerID_i[lab] = labToMarkerID[real_marker_offset[i]+lab];
    }

    bool im_left_handed = is_left_handed[i];

    calculateDegreesOfFreedom((int *)generationGap_i, (problemCounts[i]+1), labToMarkerID_i, im_left_handed);

    // Now we populate the big generationGap with the generationGap_i for this subject.
    for (int lab1=0; lab1<problemCounts[i]; lab1++)
    {
      for (int lab2=0; lab2<problemCounts[i]; lab2++)
      {
        generationGap[lab1+real_marker_offset[i]][lab2+real_marker_offset[i]] = generationGap_i[lab1][lab2];
      }
    }
  } // end for(...)
} // end rssd::calculateGenerationGaps(...)

// Before entering the algorithm that iteratively moves towards an assignment, we will calculate the "compatibility score"
// between each object-object pair and each label-label pair. E.g., how compatible is object i being label l with object j
// being label l', for all i,j within objects and all l, l' within labels. These scores are used to give support for an 
// object-label assignment.

/// @todo Use generation gap to filter inter-marker distance calculations.
void rssd::calculateCompatibilityScores(
  const rssd::marker_v &objects,
  const rssd::marker_v &labels,
  const rssd::marker_v &reference_labels);
{
  // Local vars
	int z_10th = 0;
	int z_100th = 0;
  float normDist = 0.0f;
  float compat = 0.0f;
  float statCompat = 0.0f;
  float statIncompat = 0.0f;
  float incompat = 0.0f; 
	float distDiff = 0.0f;
  /// @note The difference in the distances.
	float diff = 0.0f; 
  /// @note The cutoff, where "error ratios" above this cutoff go from positive to 0 compatibility.
  const float ratio_cutoff = rssd::compatibility_g::RATIO_CUTOFF_GLOBAL;

  // Calculate inter-label distances
  marker_distance_m label_distances;
  calculateInterLabelDistances(
    labels,
    label_distances);

  // Calculate inter-object distances
  marker_distance_m object_distances;
  calculateInterObjectDistances(
    objects,
    object_distances);

  // Calculate inter-label distances
  marker_distance_m reference_label_distances;
  calculateInterLabelDistances(
    reference_labels,
    reference_label_distances);

  // Parse unique object-label/object-label pairs
  /// @note This is done to flatten the main loop.
  compatibility_m compatibilities;
  getUniqueCompatabilityPairs(
    objects,
    labels,
    compatibilities);

  // Main loop for calculating object-label compatibility
  compatibility_m::iterator
    iter = compatibilities.begin(),
    end = compatibilities.end();
  for (; iter != end; ++iter)
  {
    compatibility_t &compatibility = iter->second;

    // Reset loop vars
    compat = 0.0f;
    incompat = 0.0f; 
    statCompat = 0.0f;
    statIncompat = 0.0f;

    // Get object/label references
    const marker_t &this_object = compatibility.first_pair.first_marker;
    const marker_t &this_label = compatibility.first_pair.second_marker;
    const marker_t &this_reference_label = reference_labels[compatibility.first_pair.second_marker.index];
    const marker_t &other_object = compatibility.second_pair.first_marker;
    const marker_t &other_label = compatibility.second_pair.second_marker;
    const marker_t &other_reference_label = reference_labels[compatibility.first_pair.second_marker.index];

    /// @assert These combinations should be filtered out when parsing unique compatibility pairs.
    assert (this_label != other_label); 

    // Create object/label pairs
    marker_pair_t object_pair(this_object, other_object);
    marker_pair_t label_pair(this_label, other_label);
    marker_pair_t reference_label_pair(this_reference_label, other_reference_label);

    /// @assert Confirm that our object pairs are valid for this iteration.
    assert (object_distances.find(object_pair) != object_distances.end());
    assert (label_distances.find(label_pair) != label_distances.end());
    assert (reference_label_distances.find(label_pair) != reference_label_distances.end());

    // Get object/label distances
    const float object_distance = object_distances[object_pair];
    const float label_distance = label_distances[label_pair];
    const float reference_label_distance = reference_label_distances[reference_label_pair];

    /// @note Previously: if (obj_has_final_assignment[this_object.index] && obj_has_final_assignment[other_object.index])
    /// @todo ASK MANNY
    ///       @li When do objects get their final assignment?
    ///           Is this relevant for a test case?
    if (this_object.assigned && other_object.assigned)
    {
      compatibility.clear();
      continue;
    }
    else if (generationGap[this_label.index][other_label.index] == -1) 
    {
      compatibility.clear();
      continue;
    }
    else if (generationGap[this_label.index][other_label.index] == 0) 
    {
      /// @note Previously: o_dist[this_object.index][other_object.index] - p_dist[this_label.index][other_label.index];
      float distDiffP = std::abs(object_distance - reference_label_distance); 

      // Calculate compatibility scores based on object-label distance
      if (distDiffP > (0.5f * reference_label_distance)) 
      {
        compat = rssd::compatibility_t::RIGID_VIOL_COMPAT;
        incompat = std::pow((double)RIGID_VIOL_INCOMPAT, 2) + RIGID_VIOL_INCOMPAT;
      }
      else if (distDiffP > (0.29f * reference_label_distance)) 
      {
        float violRatio = distDiffP / (0.5f * reference_label_distance);
        compat = violRatio * rssd::compatibility_t::RIGID_VIOL_COMPAT;
        incompat = violRatio * rssd::compatibility_t::RIGID_VIOL_INCOMPAT;
        incompat = std::pow((double)incompat, 2) + incompat;
      }

      // Assign compatibility scores for the given object-label pair
      compatibility.compatibility_score = compat;
      compatibility.incompatibility_score = incompat;
      continue;
    }

    /// @todo ASK MANNY
    ///       @li What is being calculated for diff_point?
    ///       @li Why divide by label_component_distance?
    point_t label_component_distance = this_label.position - other_label.position;
    point_t diff_point = (this_object.position + label_component_distance) - other_object.position;
    /// @note Previously: diff = diff / l_dist[this_label.index][other_label.index];
    diff = diff_point.length() / label_component_distance;
    
    /// @todo ASK MANNY
    ///       @li What is sd[][]?
    ///       @li What is being calculated?
    /// @note Previously: o_dist[this_object.index][other_object.index] - l_dist[this_label.index][other_label.index];
    distDiff = std::abs(object_distance - label_distance); 
    distDiff = distDiff / sd[this_label.index][other_label.index];

    /// @todo ASK MANNY
    ///       @li What is the difference between statCompat/compat and statIncompat/incompat?
    ///           Are they both accumulators for the same compatibility value calculations?
    z_10th = (int)(distDiff * 10.0f);
    z_100th = (int)(100.0f * distDiff + 0.5f) % 10;
    if ((normDist = NormalDistributionTable::get(z_10th, z_100th)) != 0.0f) 
      statCompat = 2.0f * compatibility_g::RATIO_CUTOFF_GLOBAL * (1.0f - (normDist - 0.25f));
    statIncompat = compatibility_g::RATIO_CUTOFF_GLOBAL;
    if (statCompat > 0.0f) 
      statIncompat = std::max(0.0f, (statIncompat - statCompat));

    // Calculate compatibility scores
    /// @todo ASK MANNY
    ///       @li Where did the compat/incompat formulae come from?
    compat = std::max(0.0f, (compatibility_g::RATIO_CUTOFF_GLOBAL - diff));
    compat = (compat * statCompat) + (0.5f * compat) + (0.5f * statCompat);
    incompat = (diff * statIncompat) + (0.5f * diff) + (0.5f * statIncompat);

    // Assign compatibility scores for the given object-label pair
    compatibility.compatibility_score = compat;
    compatibility.incompatibility_score = incompat;
  } // end for(...)
} // end calculateCompatibilityScores(...)
#endif
