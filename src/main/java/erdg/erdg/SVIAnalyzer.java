package erdg.erdg;

import java.util.HashSet;
import java.util.Set;

/**
 * Detects Shared Variable Interference (SVI) between pairs of message servers.
 *
 * Java port of the have_svi method from ERDGTestGenerator in Python.
 *
 * Two message servers m1 and m2 of the same rebec r have SVI iff they share
 * access to at least one state variable v such that at least one of the
 * accesses is a write:
 *   - W-W: both write v
 *   - W-R: m1 writes v, m2 reads v
 *   - R-W: m1 reads v,  m2 writes v
 *
 * Extracted directly from E_write and E_read edges; no dedicated SVI edge
 * type is needed.
 */
public class SVIAnalyzer {

    private final ERDG erdg;

    public SVIAnalyzer(ERDG erdg) {
        this.erdg = erdg;
    }

    /**
     * Return true if ms1 and ms2 (each in "instance.method" form) exhibit SVI.
     */
    public boolean haveSvi(String ms1, String ms2) {
        Set<String> writesMs1 = writesOf(ms1);
        Set<String> writesMs2 = writesOf(ms2);
        Set<String> readsMs1  = readsOf(ms1);
        Set<String> readsMs2  = readsOf(ms2);

        if (intersects(writesMs1, writesMs2)) return true;  // W-W
        if (intersects(writesMs1, readsMs2))  return true;  // W-R
        if (intersects(readsMs1,  writesMs2)) return true;  // R-W
        return false;
    }

    // ---- helpers ----

    /** All variables written by the given message server. */
    private Set<String> writesOf(String ms) {
        Set<String> result = new HashSet<>();
        for (var e : erdg.E_write) {
            if (e.source().equals(ms)) result.add(e.target());
        }
        return result;
    }

    /** All variables read by the given message server. */
    private Set<String> readsOf(String ms) {
        Set<String> result = new HashSet<>();
        for (var e : erdg.E_read) {
            if (e.target().equals(ms)) result.add(e.source());
        }
        return result;
    }

    private static boolean intersects(Set<String> a, Set<String> b) {
        Set<String> smaller = a.size() < b.size() ? a : b;
        Set<String> larger  = a.size() < b.size() ? b : a;
        for (String s : smaller) {
            if (larger.contains(s)) return true;
        }
        return false;
    }
}