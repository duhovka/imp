#!/bin/sh

# Update the copy of PMI inside the IMP repository.

# See http://integrativemodeling.org/nightly/doc/manual/subtree.html
# for more details.

# This is similar to 'git subtree pull' but makes the commit a little
# differently (subtree uses git commit-tree followed by merge; we just
# cherry-pick and squash the commits). git subtree results in two commits,
# and "git show" and GitHub can get very confused by the tree commit,
# thinking it modifies every file in IMP. On the other hand, cherry-picking
# gives a single "small" commit. The commit message is formatted in the
# same way as that from "git subtree" so using this script should not
# break future "git subtree pull" or "git subtree push" invocations.

# Some logic is borrowed from the git-subtree shell script, which is GPLv2
# and Copyright (C) 2009 Avery Pennarun <apenwarr@gmail.com>


ensure_clean()
{
    if ! git diff-index HEAD --exit-code --quiet 2>&1; then
        die "Working tree has modifications.  Cannot add."
    fi
    if ! git diff-index --cached HEAD --exit-code --quiet 2>&1; then
        die "Index has modifications.  Cannot add."
    fi
}

find_latest_squash()
{
        dir="$1"
        sq=
        main=
        sub=
        git log --grep="^git-subtree-dir: $dir/*\$" \
                --pretty=format:'START %H%n%s%n%n%b%nEND%n' HEAD |
        while read a b junk; do
                case "$a" in
                        START) sq="$b" ;;
                        git-subtree-mainline:) main="$b" ;;
                        git-subtree-split:) sub="$b" ;;
                        END)
                                if [ -n "$sub" ]; then
                                        if [ -n "$main" ]; then
                                                # a rejoin commit?
                                                # Pretend its sub was a squash.
                                                sq="$sub"
                                        fi
                                        echo "$sq" "$sub"
                                        break
                                fi
                                sq=
                                main=
                                sub=
                                ;;
                esac
        done
}

squash_msg()
{
        dir="$1"
        oldsub="$2"
        newsub="$3"
        newsub_short=$(git rev-parse --short "$newsub")

        if [ -n "$oldsub" ]; then
                oldsub_short=$(git rev-parse --short "$oldsub")
                echo "Squashed '$dir/' changes from $oldsub_short..$newsub_short"
                echo
                git log --pretty=tformat:'%h %s' "$oldsub..$newsub"
                git log --pretty=tformat:'REVERT: %h %s' "$newsub..$oldsub"
        else
                echo "Squashed '$dir/' content from commit $newsub_short"
        fi

        echo
        echo "git-subtree-dir: $dir"
        echo "git-subtree-split: $newsub"
}

new_squash_commit()
{
    dir="$1"
    old="$2"
    oldsub="$3"
    newsub="$4"
    oldhead=$(git rev-parse HEAD)
    # Get all subtree commits, in the order they were made
    # (this is the reverse of the log order)
    revs=$(git log --pretty=tformat:'%H' "$oldsub..$newsub"|tac)
    # Cherry-pick into the subtree directory, but don't make a commit (-n)
    git cherry-pick -n --strategy=recursive -Xsubtree="$dir" $revs || exit $?
    # Commit the combination of all the cherry-picked changes
    if [ -n "$old" ]; then
        squash_msg "$dir" "$oldsub" "$newsub" | git commit -n -F - || exit $?
    else
        squash_msg "$dir" "" "$newsub" | git commit -n -F - || exit $?
    fi
}

merge()
{
    local dir=$1
    local rev=$(git rev-parse FETCH_HEAD)
    first_split="$(find_latest_squash "$dir")"
    if [ -z "$first_split" ]; then
        die "Can't squash-merge: '$dir' was never added."
    fi
    set $first_split
    old=$1
    sub=$2
    if [ "$sub" = "$rev" ]; then
        echo "Subtree is already at commit $rev."
        exit 0
    fi
    new_squash_commit "$dir" "$old" "$sub" "$rev" || exit $?
}

pull()
{
    local prefix=$1
    local refspec=$2
    local branch=$3
    ensure_clean
    git fetch $refspec $branch || exit $?
    merge $1
}

pull modules/pmi git@github.com:salilab/pmi.git develop
